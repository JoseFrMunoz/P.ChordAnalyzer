#include "FilaNombre.h"
#include "Tema.h"
#include "../Analysis/KeyNames.h"

namespace filaNombre
{
    void dibujar (juce::Graphics& g, juce::Rectangle<float> zona, const pca::KeyResult& r)
    {
        if (! r.hayEscala)
        {
            g.setColour (tema::color::textoSecundario);
            g.setFont (tema::inter (15.0f));
            g.drawText (juce::String (juce::CharPointer_UTF8 ("Escuchando\xe2\x80\xa6")), zona, juce::Justification::centredLeft, false);
            return;
        }

        // Confianza a la derecha, en mono.
        const auto porcentaje = juce::String (juce::roundToInt (r.confianza * 100.0f)) + " %";
        g.setColour (tema::color::textoTerciario);
        g.setFont (tema::mono (12.0f));
        g.drawText (porcentaje, zona, juce::Justification::centredRight, false);

        // Nombre en español + cifrado inglés, alineados a la izquierda.
        juce::AttributedString texto;
        texto.append (pca::nombres::nombreEspanol (r.tonica, r.menor) + "  ",
                      juce::Font (tema::inter (22.0f, tema::Peso::negrita)), tema::color::textoPrincipal);
        texto.append (pca::nombres::cifrado (r.tonica, r.menor),
                      juce::Font (tema::mono (13.0f)), tema::color::textoSecundario);
        texto.setJustification (juce::Justification::centredLeft);
        texto.setWordWrap (juce::AttributedString::none);
        texto.draw (g, zona.withTrimmedRight (50.0f));
    }
}
