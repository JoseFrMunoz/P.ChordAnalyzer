#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "BotonIcono.h"

/**
    Cabecera de 36 px: título "P.CHORDANALYZER", etiqueta "BYPASS" cuando
    procede y los botones de congelar y bypass a la derecha.
*/
class Cabecera final : public juce::Component
{
public:
    Cabecera();

    void setBypass (bool activo);

    void paint (juce::Graphics&) override;
    void resized() override;

    BotonIcono botonCongelar { "Congelar", BotonIcono::Icono::congelar };
    BotonIcono botonBypass   { "Bypass",   BotonIcono::Icono::bypass };

private:
    bool bypass = false;
};
