#include "KeyDetector.h"
#include "KeyProfiles.h"
#include <algorithm>
#include <cmath>

namespace pca
{
    namespace
    {
        float limitar01 (float v) noexcept { return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v); }

        /** Correlación de Pearson entre dos vectores de 12 valores. */
        float pearson (const std::array<float, 12>& x, const std::array<float, 12>& y) noexcept
        {
            float mediaX = 0.0f, mediaY = 0.0f;
            for (size_t i = 0; i < 12; ++i) { mediaX += x[i]; mediaY += y[i]; }
            mediaX /= 12.0f;
            mediaY /= 12.0f;

            float sxy = 0.0f, sxx = 0.0f, syy = 0.0f;
            for (size_t i = 0; i < 12; ++i)
            {
                const auto dx = x[i] - mediaX;
                const auto dy = y[i] - mediaY;
                sxy += dx * dy;
                sxx += dx * dx;
                syy += dy * dy;
            }

            const auto den = std::sqrt (sxx * syy);
            return den > 1.0e-12f ? sxy / den : 0.0f;
        }

        /** Coeficiente de una media exponencial con constante de tiempo tau. */
        float coeficiente (double segundos, double tau) noexcept
        {
            return (float) (1.0 - std::exp (-segundos / tau));
        }

        /**
            Convierte la diferencia entre la mejor y la segunda correlación en
            una confianza de 0 a 1. Una diferencia de 0,15 o más ya se considera
            seguridad total (valor ajustado con los tests sintéticos).
        */
        float confianzaDesdeDiferencia (float diferencia) noexcept
        {
            return limitar01 (diferencia / 0.15f);
        }
    }

    KeyDetector::KeyDetector (Perfiles tipo)
    {
        const auto& mayor = tipo == Perfiles::temperley ? perfiles::temperleyMayor : perfiles::krumhanslMayor;
        const auto& menor = tipo == Perfiles::temperley ? perfiles::temperleyMenor : perfiles::krumhanslMenor;

        // Perfil de cada escala: el perfil base rotado para que empiece en su tónica.
        for (int tonica = 0; tonica < 12; ++tonica)
        {
            for (int nota = 0; nota < 12; ++nota)
            {
                const auto grado = (size_t) ((nota - tonica + 12) % 12);
                perfilesRotados[(size_t) tonica][(size_t) nota]      = mayor[grado];
                perfilesRotados[(size_t) tonica + 12][(size_t) nota] = menor[grado];
            }
        }

        reset();
    }

    void KeyDetector::reset() noexcept
    {
        acumulado.fill (0.0f);
        visual.fill (0.0f);
        correlaciones.fill (0.0f);
        hayEscala = false;
        escalaMostrada = 0;
        candidata = -1;
        tiempoCandidata = 0.0;
        confianza = 0.0f;
        silencio = true;
        tramasConSenal = 0;
    }

    void KeyDetector::processFrame (const Cromagrama& croma, float rms, double segundosTrama) noexcept
    {
        const auto umbralRms = std::pow (10.0f, umbralSilencioDb / 20.0f);

        // Silencio: se congela todo (ni se borra el acumulado ni avanza la histéresis).
        if (rms < umbralRms)
        {
            silencio = true;
            return;
        }

        silencio = false;

        // Al empezar no hay historia: hasta llenar la memoria usamos la media
        // de todas las tramas (1/n) en vez de la exponencial, que daría
        // demasiado peso a los primeros acordes.
        ++tramasConSenal;
        const auto a = std::max (coeficiente (segundosTrama, memoriaSegundos), 1.0f / (float) tramasConSenal);
        const auto aVisual = coeficiente (segundosTrama, memoriaVisualSegundos);

        for (size_t i = 0; i < 12; ++i)
        {
            acumulado[i] += a * (croma[i] - acumulado[i]);
            visual[i]    += aVisual * (croma[i] - visual[i]);
        }

        calcularCorrelaciones();

        // Mejor y segunda mejor escala.
        int mejor = 0;
        for (int i = 1; i < 24; ++i)
            if (correlaciones[(size_t) i] > correlaciones[(size_t) mejor])
                mejor = i;

        // Histéresis: la candidata tiene que ser la mejor 1,5 s seguidos y,
        // si ya hay escala, ganarle a la mostrada por un margen mínimo.
        const auto supera = ! hayEscala
                         || (mejor != escalaMostrada
                             && correlaciones[(size_t) mejor] - correlaciones[(size_t) escalaMostrada] >= margenCambio);

        if (! supera)
        {
            candidata = -1;
            tiempoCandidata = 0.0;
        }
        else
        {
            if (mejor == candidata)
                tiempoCandidata += segundosTrama;
            else
            {
                candidata = mejor;
                tiempoCandidata = segundosTrama;
            }

            if (tiempoCandidata >= histeresisSegundos)
            {
                escalaMostrada = mejor;
                hayEscala = true;
                candidata = -1;
                tiempoCandidata = 0.0;
            }
        }

        // Confianza de la escala mostrada frente a la mejor de las demás.
        if (hayEscala)
        {
            float mejorOtra = -1.0f;
            for (int i = 0; i < 24; ++i)
                if (i != escalaMostrada)
                    mejorOtra = std::max (mejorOtra, correlaciones[(size_t) i]);

            const auto objetivo = confianzaDesdeDiferencia (correlaciones[(size_t) escalaMostrada] - mejorOtra);
            confianza += coeficiente (segundosTrama, suavizadoConfianzaSegundos) * (objetivo - confianza);
        }
    }

    void KeyDetector::calcularCorrelaciones() noexcept
    {
        for (size_t i = 0; i < 24; ++i)
            correlaciones[i] = pearson (acumulado, perfilesRotados[i]);
    }

    KeyResult KeyDetector::getResult() const noexcept
    {
        KeyResult r;
        r.hayEscala = hayEscala;
        r.tonica = escalaMostrada % 12;
        r.menor = escalaMostrada >= 12;
        r.confianza = confianza;
        r.silencio = silencio;

        // El cromagrama visual se normaliza para que la nota más fuerte llegue arriba.
        float maximo = 0.0f;
        for (auto v : visual)
            maximo = std::max (maximo, v);

        for (size_t i = 0; i < 12; ++i)
            r.cromagrama[i] = maximo > 0.0f ? visual[i] / maximo : 0.0f;

        return r;
    }
}
