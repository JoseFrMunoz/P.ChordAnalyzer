#include <juce_core/juce_core.h>
#include <algorithm>
#include <array>
#include "../Source/Analysis/AnalysisCore.h"
#include "../Source/Analysis/AnalysisEngine.h"
#include "../Source/Analysis/KeyNames.h"
#include "SyntheticAudio.h"

using namespace sintetico;

namespace
{
    /** Pasa todo el audio por un AnalysisCore en bloques de 512 (como haría el host). */
    pca::KeyResult analizar (pca::AnalysisCore& core, const std::vector<float>& audio)
    {
        for (size_t i = 0; i < audio.size(); i += 512)
            core.pushSamples (audio.data() + i, (int) std::min<size_t> (512, audio.size() - i));
        return core.getResult();
    }

    juce::String describir (const pca::KeyResult& r)
    {
        if (! r.hayEscala) return "(sin escala)";
        return pca::nombres::cifrado (r.tonica, r.menor) + " (" + juce::String (juce::roundToInt (r.confianza * 100)) + " %)";
    }

    /** Las 4 mejores correlaciones, para entender un fallo. */
    juce::String top4 (const pca::AnalysisCore& core)
    {
        const auto& c = core.getDetector().getCorrelaciones();
        std::array<int, 24> idx {};
        for (int i = 0; i < 24; ++i) idx[(size_t) i] = i;
        std::sort (idx.begin(), idx.end(), [&] (int a, int b) { return c[(size_t) a] > c[(size_t) b]; });

        juce::String s;
        for (int k = 0; k < 4; ++k)
            s << pca::nombres::cifrado (idx[(size_t) k] % 12, idx[(size_t) k] >= 12) << "=" << juce::String (c[(size_t) idx[(size_t) k]], 3) << " ";
        return s;
    }

    /** Progresiones típicas en mayor. Grados en semitonos desde la tónica. */
    std::vector<Evento> progresionMayor (int tonica, int variante)
    {
        const auto t = octavaMedia (tonica);
        const auto I  = triada (t, false);
        const auto IV = triada (t + 5, false);
        const auto V  = triada (t + 7, false);
        const auto vi = triada (t + 9 - 12, true);

        if (variante == 0) return { { I, 2.0 }, { IV, 2.0 }, { V, 2.0 }, { I, 2.0 } };   // I-IV-V-I
        return { { I, 2.0 }, { V, 2.0 }, { vi, 2.0 }, { IV, 2.0 } };                     // I-V-vi-IV
    }

    /** Progresiones típicas en menor. */
    std::vector<Evento> progresionMenor (int tonica, int variante)
    {
        const auto t = octavaMedia (tonica);
        const auto i   = triada (t, true);
        const auto iv  = triada (t + 5, true);
        const auto V   = triada (t + 7, false);           // dominante de la menor armónica
        const auto VI  = triada (t + 8 - 12, false);
        const auto VII = triada (t + 10 - 12, false);

        if (variante == 0) return { { i, 2.0 }, { iv, 2.0 }, { V, 2.0 }, { i, 2.0 } };   // i-iv-V-i
        return { { i, 2.0 }, { VI, 2.0 }, { VII, 2.0 }, { i, 2.0 } };                    // i-VI-VII-i
    }

    /** Escala melódica (una nota cada vez) que empieza y acaba en la tónica. */
    std::vector<Evento> melodiaEscala (int tonica, bool menor)
    {
        const std::vector<int> mayor { 0, 2, 4, 5, 7, 9, 11, 12, 11, 9, 7, 5, 4, 2, 0 };
        const std::vector<int> menorArm { 0, 2, 3, 5, 7, 8, 11, 12, 11, 8, 7, 5, 3, 2, 0 };
        const auto& grados = menor ? menorArm : mayor;

        std::vector<Evento> salida;
        const auto t = octavaMedia (tonica);
        for (auto g : grados)
            salida.push_back ({ { t + g }, 0.35 });
        salida.push_back ({ { t, t - 12 }, 1.0 });   // tónica larga al final
        return salida;
    }
}

//==============================================================================
class KeyDetectionTests final : public juce::UnitTest
{
public:
    KeyDetectionTests() : juce::UnitTest ("Deteccion de escala", "Analisis") {}

    void runTest() override
    {
        probarProgresiones (pca::KeyDetector::Perfiles::krumhansl, 48000.0, true);
        probarProgresiones (pca::KeyDetector::Perfiles::krumhansl, 44100.0, true);
        compararPerfiles();
        probarMelodias();
        probarPrimeraEscalaRapida();
        probarSilencio();
        probarHisteresis();
        probarMotorConHilo();
        probarNombres();
    }

private:
    /** Cuenta aciertos en 24 escalas × 2 progresiones. Si exigir, cada caso es un test. */
    int probarProgresiones (pca::KeyDetector::Perfiles perfiles, double fs, bool exigir)
    {
        if (exigir)
            beginTest ("Progresiones en las 24 escalas a " + juce::String (fs / 1000.0, 1) + " kHz");

        int aciertos = 0;

        for (int menor = 0; menor < 2; ++menor)
            for (int tonica = 0; tonica < 12; ++tonica)
                for (int variante = 0; variante < 2; ++variante)
                {
                    pca::AnalysisCore core (perfiles);
                    core.prepare (fs);

                    const auto prog = menor ? progresionMenor (tonica, variante) : progresionMayor (tonica, variante);
                    const auto audio = renderizar (repetir (prog, 24.0), fs, 6, 0.2f, (unsigned) (tonica * 7 + variante));

                    // A partir del segundo 10 (loop ya estable) la escala no debe cambiar nunca.
                    const auto inicioEstable = (size_t) (10.0 * fs);
                    int cambiosEnEstable = 0;
                    int ultima = -1;
                    for (size_t i = 0; i < audio.size(); i += 512)
                    {
                        core.pushSamples (audio.data() + i, (int) std::min<size_t> (512, audio.size() - i));
                        if (i < inicioEstable) continue;
                        const auto parcial = core.getResult();
                        const auto escala = parcial.tonica + (parcial.menor ? 12 : 0);
                        if (ultima >= 0 && escala != ultima) ++cambiosEnEstable;
                        ultima = escala;
                    }
                    const auto r = core.getResult();

                    const auto ok = r.hayEscala && r.tonica == tonica && r.menor == (menor == 1) && cambiosEnEstable == 0;
                    aciertos += ok ? 1 : 0;

                    if (exigir)
                        expect (ok, "Esperado " + pca::nombres::cifrado (tonica, menor == 1)
                                        + " (variante " + juce::String (variante) + "), detectado " + describir (r)
                                        + ", cambios con loop estable: " + juce::String (cambiosEnEstable)
                                        + " | " + top4 (core));
                }

        return aciertos;
    }

    void compararPerfiles()
    {
        beginTest ("Krumhansl-Kessler acierta al menos tanto como Temperley");

        const auto temperley = probarProgresiones (pca::KeyDetector::Perfiles::temperley, 48000.0, false);
        const auto krumhansl = probarProgresiones (pca::KeyDetector::Perfiles::krumhansl, 48000.0, false);

        logMessage ("  Aciertos (de 48): Temperley " + juce::String (temperley)
                    + " · Krumhansl-Kessler " + juce::String (krumhansl));
        expectGreaterOrEqual (krumhansl, temperley);
    }

    void probarMelodias()
    {
        beginTest ("Melodias de escala (una nota a la vez)");

        const int tonicas[] { 0, 2, 4, 7, 9, 10 };   // C, D, E, G, A, B♭

        for (auto tonica : tonicas)
            for (int menor = 0; menor < 2; ++menor)
            {
                pca::AnalysisCore core;
                core.prepare (48000.0);

                const auto audio = renderizar (repetir (melodiaEscala (tonica, menor == 1), 16.0), 48000.0);
                const auto r = analizar (core, audio);

                expect (r.hayEscala && r.tonica == tonica && r.menor == (menor == 1),
                        "Melodia en " + pca::nombres::cifrado (tonica, menor == 1) + ", detectado " + describir (r));
            }
    }

    void probarPrimeraEscalaRapida()
    {
        beginTest ("La primera escala aparece en 1 s como maximo");

        for (int tonica : { 0, 2, 9 })
        {
            pca::AnalysisCore core;
            core.prepare (48000.0);

            // Solo el acorde de tónica durante 1 s.
            const auto audio = renderizar ({ { triada (octavaMedia (tonica), false), 1.0 } }, 48000.0);
            const auto r = analizar (core, audio);
            expect (r.hayEscala, "Tras 1 s debe haber escala (" + pca::nombres::cifrado (tonica, false) + ")");
        }
    }

    void probarSilencio()
    {
        beginTest ("El silencio mantiene la ultima escala");

        pca::AnalysisCore core;
        core.prepare (48000.0);

        const auto antes = analizar (core, renderizar (repetir (progresionMenor (9, 0), 12.0), 48000.0));
        expect (antes.hayEscala && antes.tonica == 9 && antes.menor, "Antes del silencio: " + describir (antes));
        expect (! antes.silencio);

        const auto durante = analizar (core, silencio (6.0, 48000.0));
        expect (durante.silencio, "Debe marcar silencio");
        expect (durante.hayEscala && durante.tonica == 9 && durante.menor, "Tras 6 s de silencio: " + describir (durante));

        // Un hueco corto y vuelve la música: sigue igual.
        const auto despues = analizar (core, renderizar (repetir (progresionMenor (9, 0), 4.0), 48000.0));
        expect (! despues.silencio);
        expect (despues.tonica == 9 && despues.menor, "Al volver: " + describir (despues));
    }

    void probarHisteresis()
    {
        beginTest ("Histeresis al modular de Do mayor a Mi mayor");

        const double fs = 48000.0;
        pca::AnalysisCore core;
        core.prepare (fs);

        // 16 s en Do mayor y 20 s en Mi mayor; vigilamos cada salto.
        auto audio = renderizar (repetir (progresionMayor (0, 1), 16.0), fs);
        const auto cambioEn = audio.size();
        const auto mi = renderizar (repetir (progresionMayor (4, 1), 20.0), fs, 6, 0.2f, 99);
        audio.insert (audio.end(), mi.begin(), mi.end());

        // Los primeros 8 s (primera vuelta del loop) son de arranque: solo han
        // sonado unos pocos acordes y la escala aún puede ajustarse.
        const auto finArranque = (size_t) (8.0 * fs);
        int cambios = 0;
        int escalaAnterior = -1;
        size_t primerCambioTrasModulacion = 0;

        for (size_t i = 0; i < audio.size(); i += 512)
        {
            core.pushSamples (audio.data() + i, (int) std::min<size_t> (512, audio.size() - i));
            const auto r = core.getResult();
            if (! r.hayEscala) continue;

            if (i + 512 >= cambioEn && i < cambioEn)
                expect (r.tonica == 0 && ! r.menor, "Antes de modular debe ser C, es " + describir (r));

            const auto escala = r.tonica + (r.menor ? 12 : 0);
            if (escala != escalaAnterior)
            {
                if (i >= finArranque && escalaAnterior >= 0)
                    ++cambios;
                logMessage ("  t=" + juce::String ((double) i / fs, 1) + " s -> " + describir (r) + " | " + top4 (core));
                if (i >= cambioEn && primerCambioTrasModulacion == 0)
                    primerCambioTrasModulacion = i;
                escalaAnterior = escala;
            }
        }

        const auto final = core.getResult();
        logMessage ("  Cambios tras el arranque: " + juce::String (cambios) + " · final " + describir (final));

        expect (final.tonica == 4 && ! final.menor, "Al final debe ser E, es " + describir (final));
        // Do → Mi: como mucho un estado intermedio (p. ej. Mi menor) y el definitivo.
        expect (cambios <= 2, "Demasiados cambios (parpadeo): " + juce::String (cambios));
        expect (primerCambioTrasModulacion >= cambioEn + (size_t) (1.5 * fs),
                "El cambio no puede llegar antes de 1,5 s tras la modulacion");
    }

    void probarMotorConHilo()
    {
        beginTest ("Motor con FIFO e hilo de analisis");

        const double fs = 48000.0;
        pca::AnalysisEngine motor;
        motor.prepare (fs);

        const auto mono = renderizar (repetir (progresionMayor (7, 0), 12.0), fs);   // Sol mayor
        juce::AudioBuffer<float> bloque (2, 4800);

        // Enviamos 100 ms de audio cada 50 ms (el doble de tiempo real).
        for (size_t i = 0; i + 4800 <= mono.size(); i += 4800)
        {
            for (int c = 0; c < 2; ++c)
                bloque.copyFrom (c, 0, mono.data() + i, 4800);
            motor.pushAudio (bloque, 2);
            juce::Thread::sleep (50);
        }
        juce::Thread::sleep (300);

        const auto r = motor.getResult();
        expect (r.hayEscala && r.tonica == 7 && ! r.menor, "Motor: esperado G, detectado " + describir (r));
        expect (motor.getVersion() > 0);

        // Bypass: no se publica nada nuevo.
        motor.setActivo (false);
        juce::Thread::sleep (200);
        const auto versionEnBypass = motor.getVersion();
        for (int k = 0; k < 10; ++k)
        {
            motor.pushAudio (bloque, 2);
            juce::Thread::sleep (20);
        }
        juce::Thread::sleep (200);
        expectEquals ((int) motor.getVersion(), (int) versionEnBypass, "En bypass no debe analizar");
        expect (motor.getResult().tonica == 7, "En bypass se conserva la ultima escala");

        motor.stop();
    }

    void probarNombres()
    {
        beginTest ("Nombres de las escalas");

        expectEquals (pca::nombres::nombreEspanol (9, true), juce::String ("La menor"));
        expectEquals (pca::nombres::cifrado (9, true), juce::String ("Am"));
        expectEquals (pca::nombres::nombreEspanol (0, false), juce::String ("Do mayor"));
        expectEquals (pca::nombres::cifrado (6, true), juce::String (juce::CharPointer_UTF8 ("F\xe2\x99\xafm")));

        pca::KeyResult r;
        r.tonica = 9; r.menor = true;
        expectEquals (r.tonicaRelativa(), 0);    // La menor → Do mayor
        r.tonica = 0; r.menor = false;
        expectEquals (r.tonicaRelativa(), 9);    // Do mayor → La menor
    }
};

static KeyDetectionTests keyDetectionTests;
