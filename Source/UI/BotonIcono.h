#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/**
    Botón cuadrado de 36 × 36 de la cabecera (congelar / bypass).
    Inactivo: fondo #1C1C20 con borde. Activo: fondo blanco e icono oscuro.
*/
class BotonIcono final : public juce::Button
{
public:
    enum class Icono { congelar, bypass };

    BotonIcono (const juce::String& nombre, Icono icono);

    void paintButton (juce::Graphics&, bool encima, bool pulsado) override;

private:
    static juce::Path iconoPausa (juce::Rectangle<float>);
    static juce::Path iconoPlay (juce::Rectangle<float>);
    static juce::Path iconoEncendido (juce::Rectangle<float>);

    Icono icono;
};
