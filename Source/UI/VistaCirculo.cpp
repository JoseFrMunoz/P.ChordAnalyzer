#include "VistaCirculo.h"
#include "Tema.h"
#include "../Analysis/KeyNames.h"

namespace
{
    /** Tónica de la mayor en la posición i del círculo (0 = C arriba, sentido horario). */
    int mayorEnPosicion (int i) { return (i * 7) % 12; }

    /** Posición en el círculo de una escala (las relativas comparten posición). */
    int posicionDe (int tonica, bool menor)
    {
        const auto mayor = menor ? (tonica + 3) % 12 : tonica;
        return (mayor * 7) % 12;   // 7 es su propio inverso módulo 12
    }

    juce::Point<float> puntoEnCirculo (juce::Point<float> centro, float radio, int posicion)
    {
        const auto angulo = juce::MathConstants<float>::twoPi * (float) posicion / 12.0f;
        return { centro.x + radio * std::sin (angulo), centro.y - radio * std::cos (angulo) };
    }
}

void VistaCirculo::setResultado (const pca::KeyResult& nuevo)
{
    if (! nuevo.difiereDe (resultado))
        return;

    // Esta vista no usa el cromagrama: solo repinta si cambia lo que muestra.
    const auto cambiaAlgoVisible = nuevo.hayEscala != resultado.hayEscala || nuevo.tonica != resultado.tonica
                                || nuevo.menor != resultado.menor
                                || juce::roundToInt (nuevo.confianza * 100.0f) != juce::roundToInt (resultado.confianza * 100.0f);
    resultado = nuevo;

    if (cambiaAlgoVisible)
        repaint();
}

void VistaCirculo::paint (juce::Graphics& g)
{
    auto zona = getLocalBounds().toFloat();
    auto zonaTexto = zona.removeFromBottom (48.0f);

    // --- Círculo ---
    const auto centro = zona.getCentre();
    const auto radioExterior = juce::jmin (zona.getWidth(), zona.getHeight()) * 0.5f - 2.0f;   // ~89 px
    const auto radioMedio = radioExterior * 0.68f;

    g.setColour (tema::color::anillo);
    g.drawEllipse (juce::Rectangle<float> (radioExterior * 2.0f, radioExterior * 2.0f).withCentre (centro), 1.0f);
    g.drawEllipse (juce::Rectangle<float> (radioMedio * 2.0f, radioMedio * 2.0f).withCentre (centro), 1.0f);

    const auto radioMayores = (radioExterior + radioMedio) * 0.5f;
    const auto radioMenores = radioMedio * 0.70f;

    const auto posDetectada = resultado.hayEscala ? posicionDe (resultado.tonica, resultado.menor) : -1;

    for (int i = 0; i < 12; ++i)
    {
        const auto mayor = mayorEnPosicion (i);
        const auto menor = (mayor + 9) % 12;

        for (int anillo = 0; anillo < 2; ++anillo)
        {
            const auto esMenor = anillo == 1;
            const auto tonica = esMenor ? menor : mayor;
            const auto texto = pca::nombres::cifrado (tonica, esMenor);
            const auto fuente = juce::Font (tema::mono (esMenor ? 9.5f : 11.0f));

            const auto punto = puntoEnCirculo (centro, esMenor ? radioMenores : radioMayores, i);
            const auto ancho = juce::GlyphArrangement::getStringWidth (fuente, texto);
            auto caja = juce::Rectangle<float> (ancho + 10.0f, esMenor ? 15.0f : 17.0f).withCentre (punto);

            const auto esDetectada = posDetectada == i && resultado.menor == esMenor;
            const auto esRelativa  = posDetectada == i && resultado.menor != esMenor;

            if (esDetectada)
            {
                g.setColour (tema::color::tonica);
                g.fillRoundedRectangle (caja, caja.getHeight() * 0.5f);
                g.setColour (tema::color::fondo);
            }
            else
            {
                g.setColour (esRelativa ? tema::color::tonica : tema::color::textoSecundario);
            }

            g.setFont (fuente);
            g.drawText (texto, caja, juce::Justification::centred, false);
        }
    }

    // --- Nombre y relativa ---
    if (! resultado.hayEscala)
    {
        g.setColour (tema::color::textoSecundario);
        g.setFont (tema::inter (13.0f));
        g.drawText (juce::String (juce::CharPointer_UTF8 ("Escuchando\xe2\x80\xa6")), zonaTexto, juce::Justification::centred, false);
        return;
    }

    g.setColour (tema::color::textoPrincipal);
    g.setFont (tema::inter (20.0f, tema::Peso::negrita));
    g.drawText (pca::nombres::nombreEspanol (resultado.tonica, resultado.menor),
                zonaTexto.removeFromTop (28.0f), juce::Justification::centred, false);

    const auto relativa = pca::nombres::nombreEspanol (resultado.tonicaRelativa(), ! resultado.menor);
    const auto porcentaje = juce::String (juce::roundToInt (resultado.confianza * 100.0f)) + " %";

    // "relativa: Do mayor · " en Inter y el porcentaje en mono.
    juce::AttributedString linea;
    linea.append ("relativa: " + relativa + juce::String (juce::CharPointer_UTF8 (" \xc2\xb7 ")),
                  juce::Font (tema::inter (12.0f)), tema::color::textoSecundario);
    linea.append (porcentaje, juce::Font (tema::mono (12.0f)), tema::color::textoTerciario);
    linea.setJustification (juce::Justification::centred);
    linea.draw (g, zonaTexto.removeFromTop (18.0f));
}
