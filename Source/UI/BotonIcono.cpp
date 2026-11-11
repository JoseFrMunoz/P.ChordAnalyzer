#include "BotonIcono.h"
#include "Tema.h"

BotonIcono::BotonIcono (const juce::String& nombre, Icono i)
    : juce::Button (nombre), icono (i)
{
    setClickingTogglesState (true);
    setMouseCursor (juce::MouseCursor::PointingHandCursor);
}

void BotonIcono::paintButton (juce::Graphics& g, bool encima, bool pulsado)
{
    const auto activo = getToggleState();
    const auto area = getLocalBounds().toFloat().reduced (0.5f);

    // Fondo y borde.
    auto fondo = activo ? tema::color::tonica : tema::color::boton;
    if (! activo && (encima || pulsado))
        fondo = fondo.brighter (pulsado ? 0.25f : 0.12f);

    g.setColour (fondo);
    g.fillRoundedRectangle (area, tema::medida::radioBoton);

    if (! activo)
    {
        g.setColour (tema::color::bordeBoton);
        g.drawRoundedRectangle (area, tema::medida::radioBoton, 1.0f);
    }

    // Icono centrado de 14 × 14.
    const auto zonaIcono = area.withSizeKeepingCentre (14.0f, 14.0f);
    const auto colorIcono = activo ? tema::color::fondo : tema::color::textoTerciario;
    g.setColour (colorIcono);

    if (icono == Icono::congelar)
    {
        // Pausa mientras se actualiza; al congelar muestra "play" (para reanudar).
        g.fillPath (activo ? iconoPlay (zonaIcono) : iconoPausa (zonaIcono));
    }
    else
    {
        g.strokePath (iconoEncendido (zonaIcono),
                      juce::PathStrokeType (1.6f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
}

juce::Path BotonIcono::iconoPausa (juce::Rectangle<float> r)
{
    juce::Path p;
    const auto ancho = r.getWidth() * 0.28f;
    p.addRoundedRectangle (r.getX() + r.getWidth() * 0.14f, r.getY() + 1.0f, ancho, r.getHeight() - 2.0f, 1.0f);
    p.addRoundedRectangle (r.getRight() - r.getWidth() * 0.14f - ancho, r.getY() + 1.0f, ancho, r.getHeight() - 2.0f, 1.0f);
    return p;
}

juce::Path BotonIcono::iconoPlay (juce::Rectangle<float> r)
{
    juce::Path p;
    p.addTriangle (r.getX() + r.getWidth() * 0.2f, r.getY() + 0.5f,
                   r.getX() + r.getWidth() * 0.2f, r.getBottom() - 0.5f,
                   r.getRight() - r.getWidth() * 0.05f, r.getCentreY());
    return p;
}

juce::Path BotonIcono::iconoEncendido (juce::Rectangle<float> r)
{
    // Arco abierto arriba + línea vertical: el símbolo clásico de encendido.
    juce::Path p;
    const auto c = r.getCentre().translated (0.0f, 0.8f);
    const auto radio = r.getWidth() * 0.45f;
    const auto hueco = juce::MathConstants<float>::pi * 0.22f;

    p.addCentredArc (c.x, c.y, radio, radio, 0.0f, hueco, juce::MathConstants<float>::twoPi - hueco, true);
    p.startNewSubPath (r.getCentreX(), r.getY() - 0.5f);
    p.lineTo (r.getCentreX(), r.getCentreY() + 0.5f);
    return p;
}
