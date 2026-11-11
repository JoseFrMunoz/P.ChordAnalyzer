#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Analysis/KeyResult.h"

/**
    Vista izquierda: una octava de teclado (7 blancas + 5 negras) con las
    notas de la escala encendidas, las de fuera apagadas y un punto oscuro en
    la tónica.
*/
class VistaTeclado final : public juce::Component
{
public:
    void setResultado (const pca::KeyResult&);
    void paint (juce::Graphics&) override;

private:
    pca::KeyResult resultado;
};
