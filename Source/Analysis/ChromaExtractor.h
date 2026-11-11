#pragma once

#include <juce_dsp/juce_dsp.h>
#include <memory>
#include <vector>
#include "KeyResult.h"

namespace pca
{
    /**
        Calcula el cromagrama de un bloque de audio (una "trama").

        Pasos: ventana de Hann → FFT → buscar picos del espectro entre ~55 Hz y
        ~5 kHz → afinar la frecuencia de cada pico por interpolación parabólica →
        sumar su energía en la clase de nota correspondiente (todas las octavas
        juntas) → normalizar a 0 … 1.

        Trabajar con picos en vez de con todos los bins evita que la "fuga" de
        la FFT reparta los graves entre notas vecinas (a 55 Hz un semitono mide
        ~3 Hz y un bin ~6 Hz).

        No es seguro para tiempo real: se usa desde el hilo de análisis.
    */
    class ChromaExtractor
    {
    public:
        /** Reserva memoria según la frecuencia de muestreo. Llamar antes de usar. */
        void prepare (double sampleRate);

        int getFrameSize() const noexcept { return tamTrama; }
        int getHopSize() const noexcept   { return tamSalto; }

        /**
            Analiza getFrameSize() muestras (la más antigua primero).
            Devuelve el cromagrama normalizado y el nivel RMS de la trama.
        */
        void process (const float* trama, Cromagrama& cromaSalida, float& rmsSalida);

        static constexpr float frecuenciaMinima = 55.0f;
        static constexpr float frecuenciaMaxima = 5000.0f;

    private:
        double frecuenciaMuestreo = 48000.0;
        int tamTrama = 0;
        int tamSalto = 0;
        int binMinimo = 1;
        int binMaximo = 1;

        std::unique_ptr<juce::dsp::FFT> fft;
        std::vector<float> ventana;
        std::vector<float> datosFft;
    };
}
