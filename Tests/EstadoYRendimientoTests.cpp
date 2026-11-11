#include <juce_core/juce_core.h>
#include "../Source/EstadoPlugin.h"
#include "../Source/Analysis/AnalysisCore.h"
#include "SyntheticAudio.h"

//==============================================================================
class EstadoTests final : public juce::UnitTest
{
public:
    EstadoTests() : juce::UnitTest ("Guardado de estado", "Plugin") {}

    void runTest() override
    {
        beginTest ("Guardar y cargar conserva vista, congelar y bypass");
        {
            for (int vista = 0; vista < 3; ++vista)
                for (int c = 0; c < 2; ++c)
                    for (int b = 0; b < 2; ++b)
                    {
                        EstadoPlugin original;
                        original.vista = vista;
                        original.congelado = c == 1;
                        original.bypass = b == 1;

                        const auto bloque = original.guardar();
                        const auto leido = EstadoPlugin::cargar (bloque.getData(), (int) bloque.getSize());

                        expectEquals (leido.vista, vista);
                        expect (leido.congelado == original.congelado);
                        expect (leido.bypass == original.bypass);
                    }
        }

        beginTest ("Datos vacios o corruptos dan el estado por defecto");
        {
            const auto vacio = EstadoPlugin::cargar (nullptr, 0);
            expectEquals (vacio.vista, 1);
            expect (! vacio.congelado && ! vacio.bypass);

            const char basura[] = "esto no es un estado";
            const auto roto = EstadoPlugin::cargar (basura, (int) sizeof (basura));
            expectEquals (roto.vista, 1);
        }

        beginTest ("Una vista fuera de rango se corrige");
        {
            const juce::String xml ("<PChordAnalyzer version=\"1\" vista=\"7\" congelado=\"1\" bypass=\"0\"/>");
            juce::MemoryBlock bloque;
            juce::MemoryOutputStream (bloque, false).writeString (xml);
            const auto leido = EstadoPlugin::cargar (bloque.getData(), (int) bloque.getSize());
            expectEquals (leido.vista, 2);
            expect (leido.congelado);
        }
    }
};

//==============================================================================
class RendimientoTests final : public juce::UnitTest
{
public:
    RendimientoTests() : juce::UnitTest ("Rendimiento del analisis", "Analisis") {}

    void runTest() override
    {
        beginTest ("El analisis usa menos del 1 % de un nucleo");

        const double fs = 48000.0;
        const double segundosAudio = 20.0;

        std::vector<sintetico::Evento> prog { { sintetico::triada (60, false), 2.0 },
                                              { sintetico::triada (65, false), 2.0 } };
        const auto audio = sintetico::renderizar (sintetico::repetir (prog, segundosAudio), fs);

        pca::AnalysisCore core;
        core.prepare (fs);

        // Bloques de 4800 muestras: lo que el hilo recoge en cada despertar (100 ms).
        const auto inicio = juce::Time::getMillisecondCounterHiRes();
        for (size_t i = 0; i < audio.size(); i += 4800)
            core.pushSamples (audio.data() + i, (int) std::min<size_t> (4800, audio.size() - i));
        const auto ms = juce::Time::getMillisecondCounterHiRes() - inicio;

        const auto cpu = 100.0 * (ms / 1000.0) / ((double) audio.size() / fs);
        logMessage ("  " + juce::String (ms, 1) + " ms para " + juce::String ((double) audio.size() / fs, 1)
                    + " s de audio -> CPU " + juce::String (cpu, 3) + " % de un nucleo");

       #if JUCE_DEBUG
        logMessage ("  (compilado en Debug: no se comprueba el limite)");
       #else
        expectLessThan (cpu, 1.0, "El analisis supera el 1 % de CPU");
       #endif
    }
};

static EstadoTests estadoTests;
static RendimientoTests rendimientoTests;
