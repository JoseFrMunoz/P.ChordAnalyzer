#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Analysis/KeyResult.h"

/**
    Fila superior de las vistas de teclado y cromagrama:
    "La menor" (22 px, negrita) + "Am" (mono, gris) y la confianza a la derecha.
*/
namespace filaNombre
{
    constexpr float alto = 30.0f;

    void dibujar (juce::Graphics&, juce::Rectangle<float> zona, const pca::KeyResult&);
}
