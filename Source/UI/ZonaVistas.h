#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "VistaCirculo.h"

/**
    Zona de 230 px donde viven las vistas.

    Fase 3: solo contiene el círculo de quintas. En la fase 4 se convierte en
    el carrusel de tres vistas con flechas.

    En bypass se atenúa al 25 % con un fundido de ~200 ms. El timer de la
    animación solo corre mientras dura el fundido.
*/
class ZonaVistas final : public juce::Component,
                         private juce::Timer
{
public:
    ZonaVistas();

    void setResultado (const pca::KeyResult&);

    /** Atenúa (bypass) o restaura la opacidad. Con animar = false, salto directo. */
    void setAtenuada (bool atenuada, bool animar = true);

    void resized() override;

    static constexpr float opacidadBypass = 0.25f;
    static constexpr double duracionFundidoMs = 200.0;

private:
    void timerCallback() override;

    VistaCirculo circulo;

    float opacidadInicial = 1.0f;
    float opacidadObjetivo = 1.0f;
    double inicioFundido = 0.0;
};
