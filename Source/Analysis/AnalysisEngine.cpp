#include "AnalysisEngine.h"

namespace pca
{
    AnalysisEngine::AnalysisEngine() : juce::Thread ("P.ChordAnalyzer analisis") {}

    AnalysisEngine::~AnalysisEngine()
    {
        stop();
    }

    void AnalysisEngine::prepare (double sampleRate)
    {
        stop();

        // FIFO de 1 segundo: el hilo lo vacía cada 100 ms, así que sobra margen.
        const auto capacidad = juce::jmax (8192, (int) sampleRate);
        bufferFifo.assign ((size_t) capacidad, 0.0f);
        bufferLectura.assign ((size_t) capacidad, 0.0f);
        fifo.setTotalSize (capacidad);
        fifo.reset();

        core.prepare (sampleRate);

        preparado = true;
        startThread (juce::Thread::Priority::low);
    }

    void AnalysisEngine::stop()
    {
        preparado = false;
        stopThread (2000);
    }

    void AnalysisEngine::pushAudio (const juce::AudioBuffer<float>& buffer, int numCanales) noexcept
    {
        if (! preparado.load() || ! activo.load() || numCanales <= 0)
            return;

        const auto numMuestras = buffer.getNumSamples();
        const auto* izquierdo = buffer.getReadPointer (0);
        const auto* derecho = numCanales > 1 ? buffer.getReadPointer (1) : izquierdo;

        // Si el FIFO está lleno, write() escribe solo lo que cabe (se pierde
        // audio del análisis, nunca del sonido).
        const auto escritura = fifo.write (numMuestras);
        int origen = 0;

        escritura.forEach ([&] (int indice)
        {
            bufferFifo[(size_t) indice] = 0.5f * (izquierdo[origen] + derecho[origen]);
            ++origen;
        });
    }

    void AnalysisEngine::setActivo (bool nuevo) noexcept
    {
        // Se llama desde el hilo de audio en cada bloque: solo un atómico, sin
        // despertar al hilo (se despierta solo cada 100 ms). Al reactivar, el
        // hilo descarta el audio viejo que quedara en el FIFO.
        activo.store (nuevo, std::memory_order_relaxed);
    }

    KeyResult AnalysisEngine::getResult() const
    {
        const juce::SpinLock::ScopedLockType lock (lockResultado);
        return resultado;
    }

    void AnalysisEngine::run()
    {
        bool estabaActivo = activo.load();

        while (! threadShouldExit())
        {
            wait (intervaloMs);

            if (! activo.load())
            {
                estabaActivo = false;
                continue;
            }

            const auto disponibles = fifo.getNumReady();

            if (! estabaActivo)
            {
                // Venimos de bypass: tiramos el audio acumulado antes del bypass.
                fifo.read (disponibles);
                estabaActivo = true;
                continue;
            }

            if (disponibles <= 0)
                continue;

            int destino = 0;
            fifo.read (disponibles).forEach ([&] (int indice)
            {
                bufferLectura[(size_t) destino++] = bufferFifo[(size_t) indice];
            });

            core.pushSamples (bufferLectura.data(), destino);

            const auto nuevo = core.getResult();
            bool cambiado = false;
            {
                const juce::SpinLock::ScopedLockType lock (lockResultado);
                if (nuevo.difiereDe (resultado))
                {
                    resultado = nuevo;
                    cambiado = true;
                }
            }

            if (cambiado)
                ++version;
        }
    }
}
