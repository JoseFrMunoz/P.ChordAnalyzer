#include "Tema.h"
#include "BinaryData.h"

namespace tema
{
    namespace
    {
        /**
            Carga las tipografías embebidas una sola vez y las mantiene vivas
            mientras haya alguna ventana abierta (SharedResourcePointer).
            Al registrarlas, JUCE puede usar "JetBrains Mono" como respaldo por nombre.
        */
        struct Fuentes
        {
            Fuentes()
                : interNormal (cargar (BinaryData::InterRegular_ttf, BinaryData::InterRegular_ttfSize)),
                  interSemi   (cargar (BinaryData::InterSemiBold_ttf, BinaryData::InterSemiBold_ttfSize)),
                  interNegrita(cargar (BinaryData::InterBold_ttf, BinaryData::InterBold_ttfSize)),
                  jetbrains   (cargar (BinaryData::JetBrainsMonoRegular_ttf, BinaryData::JetBrainsMonoRegular_ttfSize))
            {
            }

            static juce::Typeface::Ptr cargar (const char* datos, int tam)
            {
                return juce::Typeface::createSystemTypefaceFor (datos, (size_t) tam);
            }

            juce::Typeface::Ptr interNormal, interSemi, interNegrita, jetbrains;
        };

        Fuentes& fuentes()
        {
            // Una instancia compartida por todas las ventanas del plugin.
            static juce::SharedResourcePointer<Fuentes> instancia;
            return *instancia;
        }

        juce::String nombreRespaldo()
        {
            const auto& tf = fuentes().jetbrains;
            return tf != nullptr ? tf->getName() : juce::String();
        }
    }

    juce::FontOptions inter (float tamPx, Peso peso)
    {
        auto& f = fuentes();
        const auto& tf = peso == Peso::negrita ? f.interNegrita
                       : peso == Peso::seminegrita ? f.interSemi
                       : f.interNormal;

        return juce::FontOptions {}.withTypeface (tf)
                                   .withPointHeight (tamPx)
                                   .withFallbacks ({ nombreRespaldo() })
                                   .withFallbackEnabled (true);
    }

    juce::FontOptions mono (float tamPx)
    {
        return juce::FontOptions {}.withTypeface (fuentes().jetbrains).withPointHeight (tamPx);
    }
}
