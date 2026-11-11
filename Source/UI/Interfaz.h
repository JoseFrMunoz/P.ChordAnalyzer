#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Cabecera.h"
#include "ZonaVistas.h"

/**
    Contenido completo de la ventana: fondo, borde, cabecera, zona de vistas y
    (fase 4) selector de puntos. No sabe nada del procesador: el editor le
    pasa los datos. Así también se puede dibujar sin plugin (Tools/Capturas).
*/
class Interfaz final : public juce::Component
{
public:
    Interfaz();

    void paint (juce::Graphics&) override;
    void resized() override;

    Cabecera cabecera;
    ZonaVistas vistas;
};
