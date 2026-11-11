#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Cabecera.h"
#include "ZonaVistas.h"
#include "SelectorPuntos.h"

/**
    Contenido completo de la ventana: fondo, borde, cabecera, zona de vistas y
    selector de puntos. No sabe nada del procesador: el editor le
    pasa los datos. Así también se puede dibujar sin plugin (Tools/Capturas).
*/
class Interfaz final : public juce::Component
{
public:
    Interfaz();

    void paint (juce::Graphics&) override;
    void resized() override;

    /** Cambia de vista (flechas, puntos o estado guardado) y sincroniza los puntos. */
    void setVista (int indice, bool animar);

    /** Se llama cuando el usuario cambia de vista. */
    std::function<void (int)> onCambioVista;

    Cabecera cabecera;
    ZonaVistas vistas;
    SelectorPuntos puntos;
};
