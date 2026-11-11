#include "VistaCromagrama.h"
#include "Escala.h"
#include "FilaNombre.h"
#include "Tema.h"
#include "../Analysis/KeyNames.h"
#include <cmath>

namespace
{
    constexpr float anchoBarras = 240.0f;   // deja sitio a las flechas del carrusel
    constexpr float altoBarras  = 120.0f;
}

void VistaCromagrama::setResultado (const pca::KeyResult& nuevo)
{
    const auto cambiaFila = nuevo.hayEscala != resultado.hayEscala || nuevo.tonica != resultado.tonica
                         || nuevo.menor != resultado.menor
                         || juce::roundToInt (nuevo.confianza * 100.0f) != juce::roundToInt (resultado.confianza * 100.0f);
    resultado = nuevo;

    if (cambiaFila)
        repaint();

    // Arranca la interpolación si alguna barra tiene que moverse.
    for (size_t i = 0; i < alturas.size(); ++i)
    {
        if (std::abs (alturas[i] - resultado.cromagrama[i]) > 0.002f)
        {
            if (! isTimerRunning())
            {
                ultimoPaso = juce::Time::getMillisecondCounterHiRes();
                startTimerHz (30);
            }
            return;
        }
    }
}

void VistaCromagrama::timerCallback()
{
    const auto ahora = juce::Time::getMillisecondCounterHiRes();
    const auto dt = juce::jlimit (0.0, 0.1, (ahora - ultimoPaso) / 1000.0);
    ultimoPaso = ahora;

    const auto paso = (float) (1.0 - std::exp (-dt / suavizadoSegundos));
    bool enMovimiento = false;

    for (size_t i = 0; i < alturas.size(); ++i)
    {
        const auto diferencia = resultado.cromagrama[i] - alturas[i];
        if (std::abs (diferencia) > 0.002f)
        {
            alturas[i] += diferencia * paso;
            enMovimiento = true;
        }
        else
        {
            alturas[i] = resultado.cromagrama[i];
        }
    }

    repaint();

    if (! enMovimiento)
        stopTimer();
}

void VistaCromagrama::paint (juce::Graphics& g)
{
    auto columna = getLocalBounds().toFloat().withSizeKeepingCentre (anchoBarras, 30.0f + 16.0f + altoBarras + 18.0f);

    filaNombre::dibujar (g, columna.removeFromTop (filaNombre::alto), resultado);
    columna.removeFromTop (16.0f);

    const auto zonaBarras = columna.removeFromTop (altoBarras);
    const auto zonaEtiquetas = columna.removeFromTop (18.0f);
    const auto anchoHueco = zonaBarras.getWidth() / 12.0f;
    const auto anchoBarra = anchoHueco * 0.62f;
    const auto conBemoles = resultado.hayEscala && pca::nombres::escalaConBemoles (resultado.tonica, resultado.menor);

    g.setFont (tema::mono (9.0f));

    for (int nota = 0; nota < 12; ++nota)
    {
        const auto centroX = zonaBarras.getX() + anchoHueco * ((float) nota + 0.5f);

        const auto color = escala::esTonica (resultado, nota) ? tema::color::tonica
                         : escala::contiene (resultado, nota) ? tema::color::enEscala
                         : tema::color::fueraDeEscala;

        // Altura mínima de 2 px para que se vea la posición de cada nota.
        const auto alto = juce::jmax (2.0f, alturas[(size_t) nota] * zonaBarras.getHeight());
        const auto barra = juce::Rectangle<float> (centroX - anchoBarra * 0.5f, zonaBarras.getBottom() - alto, anchoBarra, alto);

        g.setColour (color);
        g.fillRoundedRectangle (barra, juce::jmin (2.0f, alto * 0.5f));

        g.setColour (tema::color::textoSecundario);
        g.drawText (pca::nombres::nota (nota, conBemoles),
                    juce::Rectangle<float> (anchoHueco + 4.0f, zonaEtiquetas.getHeight()).withCentre ({ centroX, zonaEtiquetas.getCentreY() }),
                    juce::Justification::centred, false);
    }
}
