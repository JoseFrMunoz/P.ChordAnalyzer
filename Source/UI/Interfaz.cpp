#include "Interfaz.h"
#include "Tema.h"

Interfaz::Interfaz()
{
    setOpaque (true);
    addAndMakeVisible (cabecera);
    addAndMakeVisible (vistas);
    addAndMakeVisible (puntos);

    puntos.setActivo (vistas.getVista());

    vistas.onCambioVista = [this] (int indice)
    {
        puntos.setActivo (indice);
        if (onCambioVista != nullptr) onCambioVista (indice);
    };

    puntos.onSeleccion = [this] (int indice)
    {
        setVista (indice, true);
        if (onCambioVista != nullptr) onCambioVista (indice);
    };
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
    zona.removeFromTop (4);
    puntos.setBounds (zona.removeFromTop (tema::medida::altoPuntos));
}

void Interfaz::setVista (int indice, bool animar)
{
    vistas.setVista (indice, animar);
    puntos.setActivo (vistas.getVista());
}
