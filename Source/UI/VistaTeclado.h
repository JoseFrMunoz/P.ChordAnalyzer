#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Analysis/KeyResult.h"
#include "Transiciones.h"

/**
    Vista izquierda: una octava de teclado (7 blancas + 5 negras) con las
    notas de la escala encendidas, las de fuera apagadas y un punto oscuro en
    la tónica. Al cambiar de escala, las teclas se encienden y apagan con un
    fundido corto.
*/
class VistaTeclado final : public juce::Component
{
public:
    VistaTeclado();

    void setResultado (const pca::KeyResult&);
    void paint (juce::Graphics&) override;

private:
    pca::KeyResult resultado;
    bool primerResultado = true;

    Transiciones<12> encendida;  ///< 1 = tecla en la escala
    Transiciones<12> tonica;     ///< 1 = tecla con el punto de tónica
    RelojAnimacion reloj;
};
