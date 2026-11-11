#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/** Chevron de 32 × 32 sin fondo (#C8C8CE) para moverse por el carrusel. */
class Flecha final : public juce::Button
{
public:
    explicit Flecha (bool haciaDerecha);
    void paintButton (juce::Graphics&, bool encima, bool pulsado) override;

private:
    bool derecha;
};
