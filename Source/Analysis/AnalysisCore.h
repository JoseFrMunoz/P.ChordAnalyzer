#pragma once

#include <vector>
#include "ChromaExtractor.h"
#include "KeyDetector.h"

namespace pca
{
    /**
        Une el extractor de cromagrama y el detector de escala: recibe muestras
        mono una tras otra, las corta en tramas solapadas (tamaño 8192, salto
        2048 a 48 kHz) y actualiza el detector.

        Es síncrono y sin hilos: lo usan tanto el hilo de análisis del plugin
        como los tests.
    */
    class AnalysisCore
    {
    public:
        explicit AnalysisCore (KeyDetector::Perfiles perfiles = KeyDetector::Perfiles::krumhansl);

        /** Reserva memoria. Conserva lo aprendido por el detector. */
        void prepare (double sampleRate);

        /** Añade muestras mono y analiza cada trama completa. */
        void pushSamples (const float* muestras, int numMuestras);

        /** Olvida la escala detectada y el audio pendiente. */
        void reset();

        KeyResult getResult() const noexcept { return detector.getResult(); }
        const KeyDetector& getDetector() const noexcept { return detector; }

    private:
        void analizarTrama();

        double frecuenciaMuestreo = 48000.0;
        ChromaExtractor extractor;
        KeyDetector detector;

        std::vector<float> trama;   ///< últimas getFrameSize() muestras
        std::vector<float> salto;   ///< muestras nuevas que aún no llenan un salto
        int muestrasEnSalto = 0;
        int muestrasEnTrama = 0;    ///< hasta llenar la primera trama
    };
}
