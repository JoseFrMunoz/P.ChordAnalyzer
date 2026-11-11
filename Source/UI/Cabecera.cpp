#include "Cabecera.h"
#include "Tema.h"

Cabecera::Cabecera()
{
    botonCongelar.setTooltip ("Congelar la lectura");
    botonBypass.setTooltip ("Bypass: detiene el análisis");
    addAndMakeVisible (botonCongelar);
    addAndMakeVisible (botonBypass);
}

void Cabecera::setBypass (bool activo)
{
    if (bypass == activo)
        return;

    bypass = activo;
    repaint();
}

void Cabecera::paint (juce::Graphics& g)
{
    // Título: 10 px, seminegrita, espaciado ancho.
    const auto fuenteTitulo = juce::Font (tema::inter (10.0f, tema::Peso::seminegrita).withKerningFactor (0.18f));
    const juce::String titulo ("P.CHORDANALYZER");

    g.setFont (fuenteTitulo);
    g.setColour (tema::color::textoSecundario);

    const auto anchoTitulo = (int) std::ceil (juce::GlyphArrangement::getStringWidth (fuenteTitulo, titulo));
    auto zona = getLocalBounds();
    g.drawText (titulo, zona.removeFromLeft (anchoTitulo + 2), juce::Justification::centredLeft, false);

    if (bypass)
    {
        // Etiqueta "BYPASS": texto oscuro sobre fondo claro.
        const auto fuenteEtiqueta = juce::Font (tema::inter (8.5f, tema::Peso::negrita).withKerningFactor (0.12f));
        const juce::String texto ("BYPASS");
        const auto ancho = juce::GlyphArrangement::getStringWidth (fuenteEtiqueta, texto) + 10.0f;

        const auto etiqueta = juce::Rectangle<float> ((float) zona.getX() + 8.0f, (float) zona.getCentreY() - 7.5f, ancho, 15.0f);
        g.setColour (tema::color::textoPrincipal);
        g.fillRoundedRectangle (etiqueta, 3.0f);

        g.setColour (tema::color::fondo);
        g.setFont (fuenteEtiqueta);
        g.drawText (texto, etiqueta, juce::Justification::centred, false);
    }
}

void Cabecera::resized()
{
    auto zona = getLocalBounds();
    const auto lado = tema::medida::ladoBoton;

    botonBypass.setBounds (zona.removeFromRight (lado).withSizeKeepingCentre (lado, lado));
    zona.removeFromRight (6);
    botonCongelar.setBounds (zona.removeFromRight (lado).withSizeKeepingCentre (lado, lado));
}
