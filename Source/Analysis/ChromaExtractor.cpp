#include "ChromaExtractor.h"
#include <cmath>

namespace pca
{
    void ChromaExtractor::prepare (double sampleRate)
    {
        frecuenciaMuestreo = sampleRate;

        // 8192 muestras a 44,1/48 kHz (~170 ms). A frecuencias más altas
        // duplicamos el tamaño para mantener la misma resolución en Hz.
        int orden = 13;
        if (sampleRate > 100000.0) ++orden;
        if (sampleRate > 200000.0) ++orden;

        tamTrama = 1 << orden;
        tamSalto = tamTrama / 4;   // 2048 a 44,1/48 kHz

        fft = std::make_unique<juce::dsp::FFT> (orden);
        datosFft.assign ((size_t) tamTrama * 2, 0.0f);

        ventana.resize ((size_t) tamTrama);
        for (int n = 0; n < tamTrama; ++n)
            ventana[(size_t) n] = 0.5f - 0.5f * std::cos (juce::MathConstants<float>::twoPi * (float) n / (float) (tamTrama - 1));

        const auto hzPorBin = sampleRate / tamTrama;
        binMinimo = juce::jmax (2, (int) std::floor (frecuenciaMinima / hzPorBin));
        binMaximo = juce::jmin (tamTrama / 2 - 2, (int) std::ceil (frecuenciaMaxima / hzPorBin));
    }

    void ChromaExtractor::process (const float* trama, Cromagrama& cromaSalida, float& rmsSalida)
    {
        jassert (fft != nullptr);

        // Nivel RMS de la trama (sin ventana) para detectar silencio.
        double suma = 0.0;
        for (int n = 0; n < tamTrama; ++n)
            suma += (double) trama[n] * trama[n];
        rmsSalida = (float) std::sqrt (suma / tamTrama);

        // Ventana + FFT. Tras la llamada, datosFft[k] es la magnitud del bin k.
        for (int n = 0; n < tamTrama; ++n)
            datosFft[(size_t) n] = trama[n] * ventana[(size_t) n];
        std::fill (datosFft.begin() + tamTrama, datosFft.end(), 0.0f);
        fft->performFrequencyOnlyForwardTransform (datosFft.data(), true);

        cromaSalida.fill (0.0f);

        // Solo cuentan los picos que no están muy por debajo del más fuerte
        // (-60 dB), para ignorar ruido y colas de la ventana.
        float magnitudMaxima = 0.0f;
        for (int k = binMinimo; k <= binMaximo; ++k)
            magnitudMaxima = juce::jmax (magnitudMaxima, datosFft[(size_t) k]);

        if (magnitudMaxima <= 0.0f)
            return;

        const auto umbral = magnitudMaxima * 0.001f;
        const auto hzPorBin = (float) (frecuenciaMuestreo / tamTrama);

        for (int k = binMinimo; k <= binMaximo; ++k)
        {
            const auto centro = datosFft[(size_t) k];
            const auto izquierda = datosFft[(size_t) k - 1];
            const auto derecha = datosFft[(size_t) k + 1];

            if (centro < umbral || centro < izquierda || centro <= derecha)
                continue;

            // Interpolación parabólica sobre la magnitud en dB: afina la
            // frecuencia real del pico entre bins.
            const auto a = std::log (izquierda + 1.0e-12f);
            const auto b = std::log (centro + 1.0e-12f);
            const auto c = std::log (derecha + 1.0e-12f);
            const auto denominador = a - 2.0f * b + c;
            const auto desplazamiento = denominador < 0.0f ? 0.5f * (a - c) / denominador : 0.0f;

            const auto frecuencia = ((float) k + desplazamiento) * hzPorBin;
            if (frecuencia < frecuenciaMinima || frecuencia > frecuenciaMaxima)
                continue;

            const auto midi = 69.0f + 12.0f * std::log2 (frecuencia / 440.0f);
            const auto claseNota = ((int) std::lround (midi) % 12 + 12) % 12;

            // Energía del pico.
            cromaSalida[(size_t) claseNota] += centro * centro;
        }

        // Normalizar para que la nota más fuerte valga 1.
        float maximo = 0.0f;
        for (auto v : cromaSalida)
            maximo = juce::jmax (maximo, v);

        if (maximo > 0.0f)
            for (auto& v : cromaSalida)
                v /= maximo;
    }
}
