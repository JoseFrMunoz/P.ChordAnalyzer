#include "SelectorPuntos.h"
#include "Tema.h"

namespace
{
    constexpr float separacion = 16.0f;   // entre centros
}

void SelectorPuntos::setActivo (int indice)
{
    if (indice == activo)
        return;

    activo = indice;
    repaint();
}

juce::Point<float> SelectorPuntos::centroPunto (int indice) const
{
    const auto centro = getLocalBounds().toFloat().getCentre();
    return { centro.x + separacion * (float) (indice - 1), centro.y };
}

int SelectorPuntos::puntoEn (juce::Point<float> p) const
{
    // Zona de clic generosa (16 × 24) alrededor de cada punto.
    for (int i = 0; i < numPuntos; ++i)
        if (std::abs (p.x - centroPunto (i).x) <= separacion * 0.5f)
            return i;
    return -1;
}

void SelectorPuntos::paint (juce::Graphics& g)
{
    for (int i = 0; i < numPuntos; ++i)
    {
        const auto esActivo = i == activo;
        const auto diametro = esActivo ? 7.0f : 6.0f;

        g.setColour (esActivo ? tema::color::tonica : tema::color::bordeBoton);
        g.fillEllipse (juce::Rectangle<float> (diametro, diametro).withCentre (centroPunto (i)));
    }
}

void SelectorPuntos::mouseUp (const juce::MouseEvent& e)
{
    const auto i = puntoEn (e.position);
    if (i >= 0 && onSeleccion != nullptr)
        onSeleccion (i);
}

void SelectorPuntos::mouseMove (const juce::MouseEvent& e)
{
    setMouseCursor (puntoEn (e.position) >= 0 ? juce::MouseCursor::PointingHandCursor : juce::MouseCursor::NormalCursor);
}

void SelectorPuntos::mouseExit (const juce::MouseEvent&)
{
    setMouseCursor (juce::MouseCursor::NormalCursor);
}
