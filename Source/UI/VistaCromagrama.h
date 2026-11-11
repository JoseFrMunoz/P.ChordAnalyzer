#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Analysis/KeyResult.h"

/**
    Vista derecha: 12 barras (C → B) con la energía de cada nota. En escala
    #EDEDF0, tónica blanca, fuera de escala #2E2E34.

    Las barras se mueven suavemente hacia el valor nuevo. El timer de la
    interpolación solo corre mientras quedan barras por llegar a su destino.
*/
class VistaCromagrama final : public juce::Component,
                              private juce::Timer
{
public:
    void setResultado (const pca::KeyResult&);
    void paint (juce::Graphics&) override;

    /** Constante de tiempo del movimiento de las barras. */
    static constexpr double suavizadoSegundos = 0.07;

private:
    void timerCallback() override;

    pca::KeyResult resultado;
    pca::Cromagrama alturas {};       ///< lo que se dibuja ahora mismo
    double ultimoPaso = 0.0;
};
