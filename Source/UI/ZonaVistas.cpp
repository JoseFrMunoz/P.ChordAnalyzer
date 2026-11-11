#include "ZonaVistas.h"

ZonaVistas::ZonaVistas()
{
    addAndMakeVisible (circulo);
}

void ZonaVistas::setResultado (const pca::KeyResult& r)
{
    circulo.setResultado (r);
}

void ZonaVistas::setAtenuada (bool atenuada, bool animar)
{
    const auto objetivo = atenuada ? opacidadBypass : 1.0f;
    if (juce::approximatelyEqual (objetivo, opacidadObjetivo) && (isTimerRunning() || juce::approximatelyEqual (getAlpha(), objetivo)))
        return;

    opacidadObjetivo = objetivo;

    if (! animar)
    {
        stopTimer();
        setAlpha (objetivo);
        return;
    }

    opacidadInicial = getAlpha();
    inicioFundido = juce::Time::getMillisecondCounterHiRes();
    startTimerHz (60);
}

void ZonaVistas::timerCallback()
{
    const auto t = juce::jlimit (0.0, 1.0, (juce::Time::getMillisecondCounterHiRes() - inicioFundido) / duracionFundidoMs);
    setAlpha (opacidadInicial + (opacidadObjetivo - opacidadInicial) * (float) t);

    if (t >= 1.0)
        stopTimer();
}

void ZonaVistas::resized()
{
    circulo.setBounds (getLocalBounds());
}
