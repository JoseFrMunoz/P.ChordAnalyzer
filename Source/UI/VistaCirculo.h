#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Analysis/KeyResult.h"

/**
    Vista central (la que se abre por defecto): círculo de quintas.

    Anillo exterior con las 12 mayores (C, G, D … F) e interior con sus
    relativas menores. La escala detectada va en una "píldora" blanca, su
    relativa en blanco y el resto en gris. Debajo, el nombre en español y la
    relativa con la confianza.
*/
class VistaCirculo final : public juce::Component
{
public:
    void setResultado (const pca::KeyResult&);
    void paint (juce::Graphics&) override;

private:
    pca::KeyResult resultado;
};
