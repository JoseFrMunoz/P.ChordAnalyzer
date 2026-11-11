#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <atomic>
#include <vector>
#include "AnalysisCore.h"

namespace pca
{
    /**
        Motor de análisis en segundo plano.

        - Hilo de audio: pushAudio() copia la mezcla mono (L+R)/2 a un FIFO sin
          bloqueos (juce::AbstractFifo). No reserva memoria, no usa locks ni logs.
        - Hilo de análisis propio: se despierta ~20 veces por segundo, vacía el
          FIFO, analiza y publica el resultado.
        - UI: getResult() devuelve una copia del último resultado. El lock que la
          protege solo lo usan el hilo de análisis y la UI, nunca el de audio.
    */
    class AnalysisEngine : private juce::Thread
    {
    public:
        AnalysisEngine();
        ~AnalysisEngine() override;

        /** Llamar desde prepareToPlay (fuera del hilo de audio). */
        void prepare (double sampleRate);

        /** Detiene el hilo (releaseResources / destructor). */
        void stop();

        /** Hilo de audio: copia el bloque al FIFO. Seguro en tiempo real. */
        void pushAudio (const juce::AudioBuffer<float>& buffer, int numCanales) noexcept;

        /** Activa o desactiva el análisis (bypass). Desactivado, CPU ≈ 0.
            Seguro en tiempo real. */
        void setActivo (bool activo) noexcept;
        bool isActivo() const noexcept { return activo.load(); }

        /** Último resultado publicado. */
        KeyResult getResult() const;

        /** Aumenta cada vez que se publica un resultado distinto. */
        juce::uint32 getVersion() const noexcept { return version.load(); }

        static constexpr int intervaloMs = 50;    ///< ~20 análisis por segundo (más fluido)

    private:
        void run() override;

        juce::AbstractFifo fifo { 1 };
        std::vector<float> bufferFifo;
        std::vector<float> bufferLectura;

        AnalysisCore core;

        std::atomic<bool> preparado { false };
        std::atomic<bool> activo { true };

        mutable juce::SpinLock lockResultado;
        KeyResult resultado;
        std::atomic<juce::uint32> version { 0 };
    };
}
