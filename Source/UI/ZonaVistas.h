#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "VistaTeclado.h"
#include "VistaCirculo.h"
#include "VistaCromagrama.h"
#include "Flecha.h"

/**
    Zona de 230 px con el carrusel de tres vistas:
    0 = teclado, 1 = círculo de quintas (por defecto), 2 = cromagrama.

    - Las tres vistas reciben siempre el mismo resultado: cambiar de vista no
      reinicia nada.
    - Cambio de vista: deslizamiento horizontal de ~380 ms con ease-out.
    - Bypass: opacidad al 25 % con un fundido de ~200 ms.
    - El timer de animación solo corre mientras hay algo animándose.
*/
class ZonaVistas final : public juce::Component,
                         private juce::Timer
{
public:
    ZonaVistas();

    void setResultado (const pca::KeyResult&);

    /** Cambia de vista (0 … 2). Con animar = false, salto directo. */
    void setVista (int indice, bool animar = true);
    int getVista() const noexcept { return vistaActual; }

    /** Se llama cuando el usuario cambia de vista con las flechas. */
    std::function<void (int)> onCambioVista;

    /** Atenúa (bypass) o restaura la opacidad. Con animar = false, salto directo. */
    void setAtenuada (bool atenuada, bool animar = true);

    void resized() override;

    static constexpr int numVistas = 3;
    static constexpr int vistaPorDefecto = 1;
    static constexpr float opacidadBypass = 0.25f;
    static constexpr double duracionFundidoMs = 200.0;
    static constexpr double duracionDeslizamientoMs = 380.0;

private:
    void timerCallback() override;
    void arrancarAnimacion();
    void colocarPista();
    void actualizarFlechas();

    /** Contenedor de las tres vistas puestas en fila; se desplaza en horizontal. */
    juce::Component pista;
    VistaTeclado teclado;
    VistaCirculo circulo;
    VistaCromagrama cromagrama;

    Flecha flechaIzquierda { false };
    Flecha flechaDerecha { true };

    int vistaActual = vistaPorDefecto;

    // Deslizamiento (en unidades de vista: 0 … 2).
    float posicion = (float) vistaPorDefecto;
    float posicionInicial = (float) vistaPorDefecto;
    double inicioDeslizamiento = 0.0;
    bool deslizando = false;

    // Fundido de bypass.
    float opacidadInicial = 1.0f;
    float opacidadObjetivo = 1.0f;
    double inicioFundido = 0.0;
    bool fundiendo = false;
};
