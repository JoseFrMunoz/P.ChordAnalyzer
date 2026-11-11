#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Analysis/KeyResult.h"
#include "Transiciones.h"

/**
    Vista central (la que se abre por defecto): círculo de quintas.

    Anillo exterior con las 12 mayores (C, G, D … F) e interior con sus
    relativas menores. La escala detectada va en una "píldora" blanca, su
    relativa en blanco y el resto en gris. Debajo, el nombre en español y la
    relativa con la confianza.

    Al cambiar de escala, la píldora vieja se apaga y la nueva se enciende
    con un fundido corto.
*/
class VistaCirculo final : public juce::Component
{
public:
    VistaCirculo();

    void setResultado (const pca::KeyResult&);
    void paint (juce::Graphics&) override;

private:
    /** Índice de etiqueta: 0-11 mayores por posición, 12-23 menores por posición. */
    void actualizarObjetivos();

    pca::KeyResult resultado;
    bool primerResultado = true;

    Transiciones<24> pildora;    ///< 1 = etiqueta con píldora blanca
    Transiciones<24> resaltado;  ///< 1 = etiqueta en blanco (relativa o detectada)
    RelojAnimacion reloj;
};
