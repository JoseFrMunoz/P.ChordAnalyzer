#include "Interfaz.h"
#include "Tema.h"

Interfaz::Interfaz()
{
    setOpaque (true);
    addAndMakeVisible (cabecera);
    addAndMakeVisible (vistas);
    setSize (tema::medida::anchoVentana, tema::medida::altoVentana);
}

void Interfaz::paint (juce::Graphics& g)
{
    // El host pinta detrás de las esquinas redondeadas: usamos el mismo
    // tono de fondo para que no se vean picos claros.
    g.fillAll (tema::color::fondo);

    g.setColour (tema::color::borde);
    g.drawRoundedRectangle (getLocalBounds().toFloat().reduced (0.5f), tema::medida::radioVentana, 1.0f);
}

void Interfaz::resized()
{
    auto zona = getLocalBounds().reduced (tema::medida::padding);

    cabecera.setBounds (zona.removeFromTop (tema::medida::altoCabecera));
    zona.removeFromTop (6);
    vistas.setBounds (zona.removeFromTop (tema::medida::altoVistas));
    // Debajo quedan 24 px para el selector de puntos (fase 4).
}
