#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/**
    Tres puntos centrados, sin texto: activo blanco de 7 px, inactivos
    #3A3A40 de 6 px. Se pueden pulsar para ir a una vista.
*/
class SelectorPuntos final : public juce::Component
{
public:
    void setActivo (int indice);
    void paint (juce::Graphics&) override;
    void mouseUp (const juce::MouseEvent&) override;
    void mouseMove (const juce::MouseEvent&) override;
    void mouseExit (const juce::MouseEvent&) override;

    std::function<void (int)> onSeleccion;

    static constexpr int numPuntos = 3;

private:
    juce::Point<float> centroPunto (int indice) const;
    int puntoEn (juce::Point<float>) const;

    int activo = 1;
};
