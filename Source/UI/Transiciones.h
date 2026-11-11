#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <array>
#include <cmath>

/**
    Conjunto de N valores (0 … 1) que se acercan suavemente a su objetivo.
    Lo usan las vistas para que los cambios de escala sean un fundido en vez
    de un salto. El componente que lo usa arranca un timer solo mientras
    haya algún valor en movimiento (enMovimiento()).
*/
template <size_t N>
class Transiciones
{
public:
    /** Constante de tiempo: ~3 veces este valor hasta llegar casi al destino. */
    static constexpr double tauSegundos = 0.06;

    void setObjetivo (size_t i, float valor) noexcept { objetivo[i] = valor; }

    /** Salta directamente al objetivo (p. ej. al abrir la ventana). */
    void saltar() noexcept { actual = objetivo; }

    float operator[] (size_t i) const noexcept { return actual[i]; }

    bool enMovimiento() const noexcept
    {
        for (size_t i = 0; i < N; ++i)
            if (std::abs (actual[i] - objetivo[i]) > 0.003f)
                return true;
        return false;
    }

    /** Avanza el tiempo. Devuelve true si algo ha cambiado. */
    bool avanzar (double segundos) noexcept
    {
        const auto paso = (float) (1.0 - std::exp (-segundos / tauSegundos));
        bool cambiado = false;

        for (size_t i = 0; i < N; ++i)
        {
            const auto d = objetivo[i] - actual[i];
            if (std::abs (d) > 0.003f) { actual[i] += d * paso; cambiado = true; }
            else if (! juce::exactlyEqual (d, 0.0f)) { actual[i] = objetivo[i]; cambiado = true; }
        }
        return cambiado;
    }

private:
    std::array<float, N> actual {}, objetivo {};
};

/** Timer de 60 fps que solo corre mientras una transición está en marcha. */
class RelojAnimacion final : private juce::Timer
{
public:
    /** La función recibe los segundos desde el último tic y devuelve si sigue animando. */
    explicit RelojAnimacion (std::function<bool (double)> f) : tic (std::move (f)) {}
    ~RelojAnimacion() override { stopTimer(); }

    void arrancar()
    {
        if (isTimerRunning())
            return;
        ultimo = juce::Time::getMillisecondCounterHiRes();
        startTimerHz (60);
    }

private:
    void timerCallback() override
    {
        const auto ahora = juce::Time::getMillisecondCounterHiRes();
        const auto dt = juce::jlimit (0.0, 0.1, (ahora - ultimo) / 1000.0);
        ultimo = ahora;
        if (! tic (dt))
            stopTimer();
    }

    std::function<bool (double)> tic;
    double ultimo = 0.0;
};
