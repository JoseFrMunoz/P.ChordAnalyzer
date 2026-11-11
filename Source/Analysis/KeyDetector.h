#pragma once

#include <array>
#include "KeyResult.h"

namespace pca
{
    /**
        Decide la escala a partir de una serie de cromagramas.

        - Acumula los cromagramas con una media exponencial de ~5 s.
        - Correlaciona el acumulado con 24 perfiles (12 mayores + 12 menores).
        - Confianza = diferencia entre la mejor correlación y la segunda.
        - Histéresis: la escala mostrada solo cambia si la nueva candidata lleva
          ≥ 1,5 s seguidos siendo la mejor y superando a la mostrada por un
          pequeño margen (así no oscila dentro de un mismo loop).
        - Silencio (< -60 dBFS): no toca nada y mantiene la última escala.

        Es código puro (sin hilos ni memoria dinámica), fácil de testear.
    */
    class KeyDetector
    {
    public:
        enum class Perfiles { krumhansl, temperley };

        explicit KeyDetector (Perfiles perfiles = Perfiles::krumhansl);

        /** Olvida todo (cromagrama acumulado y escala mostrada). */
        void reset() noexcept;

        /**
            Procesa una trama nueva.
            @param croma          cromagrama de la trama (0 … 1)
            @param rms            nivel RMS de la trama
            @param segundosTrama  tiempo que avanza esta trama (salto / fs)
        */
        void processFrame (const Cromagrama& croma, float rms, double segundosTrama) noexcept;

        /** Resultado listo para la UI. */
        KeyResult getResult() const noexcept;

        /** Correlación de cada una de las 24 escalas (0-11 mayores, 12-23 menores). Para tests. */
        const std::array<float, 24>& getCorrelaciones() const noexcept { return correlaciones; }

        static constexpr double memoriaSegundos     = 6.0;   ///< memoria de la media exponencial
        static constexpr double memoriaVisualSegundos = 0.3; ///< suavizado del cromagrama que se dibuja
        static constexpr double histeresisSegundos  = 1.5;
        static constexpr double suavizadoConfianzaSegundos = 1.0; ///< para que el % no salte
        static constexpr float  umbralSilencioDb    = -60.0f;
        /** Ventaja mínima de correlación que necesita la candidata sobre la escala mostrada. */
        static constexpr float  margenCambio        = 0.05f;

    private:
        void calcularCorrelaciones() noexcept;

        std::array<std::array<float, 12>, 24> perfilesRotados {};

        Cromagrama acumulado {};       ///< memoria larga: decide la escala
        Cromagrama visual {};          ///< memoria corta: lo que se dibuja
        std::array<float, 24> correlaciones {};

        bool hayEscala = false;
        int escalaMostrada = 0;        ///< índice 0 … 23
        int candidata = -1;
        double tiempoCandidata = 0.0;
        float confianza = 0.0f;
        bool silencio = true;
        int tramasConSenal = 0;        ///< tramas analizadas desde reset()
    };
}
