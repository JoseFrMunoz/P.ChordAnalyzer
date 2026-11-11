#include "ZonaVistas.h"

namespace
{
    /** Curva de salida suave (ease-out cúbica). */
    float easeOut (double t)
    {
        const auto inv = 1.0 - t;
        return (float) (1.0 - inv * inv * inv);
    }

    double ahoraMs() { return juce::Time::getMillisecondCounterHiRes(); }

    constexpr int ladoFlecha = 32;
}

ZonaVistas::ZonaVistas()
{
    pista.setInterceptsMouseClicks (false, true);
    pista.addAndMakeVisible (teclado);
    pista.addAndMakeVisible (circulo);
    pista.addAndMakeVisible (cromagrama);
    addAndMakeVisible (pista);

    addAndMakeVisible (flechaIzquierda);
    addAndMakeVisible (flechaDerecha);

    flechaIzquierda.onClick = [this]
    {
        setVista (vistaActual - 1);
        if (onCambioVista != nullptr) onCambioVista (vistaActual);
    };

    flechaDerecha.onClick = [this]
    {
        setVista (vistaActual + 1);
        if (onCambioVista != nullptr) onCambioVista (vistaActual);
    };

    actualizarFlechas();
}

void ZonaVistas::setResultado (const pca::KeyResult& r)
{
    teclado.setResultado (r);
    circulo.setResultado (r);
    cromagrama.setResultado (r);
}

void ZonaVistas::setVista (int indice, bool animar)
{
    indice = juce::jlimit (0, numVistas - 1, indice);
    if (indice == vistaActual && ! deslizando)
        return;

    vistaActual = indice;
    actualizarFlechas();

    if (! animar)
    {
        deslizando = false;
        posicion = (float) indice;
        colocarPista();
        return;
    }

    posicionInicial = posicion;
    inicioDeslizamiento = ahoraMs();
    deslizando = true;
    arrancarAnimacion();
}

void ZonaVistas::setAtenuada (bool atenuada, bool animar)
{
    const auto objetivo = atenuada ? opacidadBypass : 1.0f;
    if (juce::approximatelyEqual (objetivo, opacidadObjetivo)
        && (fundiendo || juce::approximatelyEqual (getAlpha(), objetivo)))
        return;

    opacidadObjetivo = objetivo;

    if (! animar)
    {
        fundiendo = false;
        setAlpha (objetivo);
        return;
    }

    opacidadInicial = getAlpha();
    inicioFundido = ahoraMs();
    fundiendo = true;
    arrancarAnimacion();
}

void ZonaVistas::arrancarAnimacion()
{
    if (! isTimerRunning())
        startTimerHz (60);
}

void ZonaVistas::timerCallback()
{
    const auto ahora = ahoraMs();

    if (deslizando)
    {
        const auto t = juce::jlimit (0.0, 1.0, (ahora - inicioDeslizamiento) / duracionDeslizamientoMs);
        posicion = posicionInicial + ((float) vistaActual - posicionInicial) * easeOut (t);
        colocarPista();

        if (t >= 1.0)
            deslizando = false;
    }

    if (fundiendo)
    {
        const auto t = juce::jlimit (0.0, 1.0, (ahora - inicioFundido) / duracionFundidoMs);
        setAlpha (opacidadInicial + (opacidadObjetivo - opacidadInicial) * (float) t);

        if (t >= 1.0)
            fundiendo = false;
    }

    if (! deslizando && ! fundiendo)
        stopTimer();
}

void ZonaVistas::colocarPista()
{
    const auto ancho = getWidth();
    pista.setBounds (-juce::roundToInt (posicion * (float) ancho), 0, ancho * numVistas, getHeight());
}

void ZonaVistas::actualizarFlechas()
{
    // Se ocultan en el extremo que no lleva a ningún sitio.
    flechaIzquierda.setVisible (vistaActual > 0);
    flechaDerecha.setVisible (vistaActual < numVistas - 1);
}

void ZonaVistas::resized()
{
    const auto ancho = getWidth();
    const auto alto = getHeight();

    teclado.setBounds (0, 0, ancho, alto);
    circulo.setBounds (ancho, 0, ancho, alto);
    cromagrama.setBounds (ancho * 2, 0, ancho, alto);
    colocarPista();

    // Flechas centradas en vertical respecto al contenido de las vistas.
    const auto y = (alto - ladoFlecha) / 2;
    flechaIzquierda.setBounds (0, y, ladoFlecha, ladoFlecha);
    flechaDerecha.setBounds (ancho - ladoFlecha, y, ladoFlecha, ladoFlecha);
}
