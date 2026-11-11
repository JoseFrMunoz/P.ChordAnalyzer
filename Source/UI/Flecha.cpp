#include "Flecha.h"
#include "Tema.h"

Flecha::Flecha (bool haciaDerecha)
    : juce::Button (haciaDerecha ? "Vista siguiente" : "Vista anterior"), derecha (haciaDerecha)
{
    setMouseCursor (juce::MouseCursor::PointingHandCursor);
}

void Flecha::paintButton (juce::Graphics& g, bool encima, bool pulsado)
{
    const auto r = getLocalBounds().toFloat().withSizeKeepingCentre (8.0f, 14.0f);

    juce::Path chevron;
    if (derecha)
    {
        chevron.startNewSubPath (r.getX(), r.getY());
        chevron.lineTo (r.getRight(), r.getCentreY());
        chevron.lineTo (r.getX(), r.getBottom());
    }
    else
    {
        chevron.startNewSubPath (r.getRight(), r.getY());
        chevron.lineTo (r.getX(), r.getCentreY());
        chevron.lineTo (r.getRight(), r.getBottom());
    }

    auto color = tema::color::textoTerciario;
    if (encima || pulsado)
        color = tema::color::tonica;

    g.setColour (color);
    g.strokePath (chevron, juce::PathStrokeType (1.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}
