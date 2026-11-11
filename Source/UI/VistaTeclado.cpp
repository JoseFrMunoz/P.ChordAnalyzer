#include "VistaTeclado.h"
#include "Escala.h"
#include "FilaNombre.h"
#include "Tema.h"

namespace
{
    constexpr float anchoTeclado = 237.0f;
    constexpr float altoTeclado  = 112.0f;

    // Clase de nota de cada tecla blanca y de cada negra (con su posición entre blancas).
    constexpr int blancas[7] { 0, 2, 4, 5, 7, 9, 11 };
    constexpr int negras[5]  { 1, 3, 6, 8, 10 };
    constexpr float posicionNegra[5] { 1.0f, 2.0f, 4.0f, 5.0f, 6.0f };   // borde derecho de la blanca n

    void dibujarPuntoTonica (juce::Graphics& g, juce::Rectangle<float> tecla, float nivel)
    {
        if (nivel <= 0.0f)
            return;

        const auto diametro = 7.0f;
        g.setColour (tema::color::fondo.withAlpha (nivel));
        g.fillEllipse (juce::Rectangle<float> (diametro, diametro)
                           .withCentre ({ tecla.getCentreX(), tecla.getBottom() - 12.0f }));
    }
}

VistaTeclado::VistaTeclado()
    : reloj ([this] (double dt)
      {
          const auto a = encendida.avanzar (dt);
          const auto b = tonica.avanzar (dt);
          if (a || b) repaint();
          return encendida.enMovimiento() || tonica.enMovimiento();
      })
{
}

void VistaTeclado::setResultado (const pca::KeyResult& nuevo)
{
    // Solo repinta si cambia la escala o el porcentaje (el cromagrama no se dibuja aquí).
    const auto cambiaEscala = nuevo.hayEscala != resultado.hayEscala || nuevo.tonica != resultado.tonica
                           || nuevo.menor != resultado.menor;
    const auto cambiaPorcentaje = juce::roundToInt (nuevo.confianza * 100.0f) != juce::roundToInt (resultado.confianza * 100.0f);
    resultado = nuevo;

    if (cambiaEscala || primerResultado)
    {
        for (int n = 0; n < 12; ++n)
        {
            encendida.setObjetivo ((size_t) n, escala::contiene (resultado, n) ? 1.0f : 0.0f);
            tonica.setObjetivo ((size_t) n, escala::esTonica (resultado, n) ? 1.0f : 0.0f);
        }

        if (primerResultado) { encendida.saltar(); tonica.saltar(); }
        else                 reloj.arrancar();
    }

    primerResultado = false;

    if (cambiaEscala || cambiaPorcentaje)
        repaint();
}

void VistaTeclado::paint (juce::Graphics& g)
{
    // Contenido centrado horizontalmente con el ancho del teclado.
    auto zona = getLocalBounds().toFloat();
    const auto anchoContenido = anchoTeclado;
    auto columna = zona.withSizeKeepingCentre (anchoContenido, 30.0f + 18.0f + altoTeclado + 26.0f);

    filaNombre::dibujar (g, columna.removeFromTop (filaNombre::alto), resultado);
    columna.removeFromTop (18.0f);

    const auto teclado = columna.removeFromTop (altoTeclado);
    const auto anchoBlanca = teclado.getWidth() / 7.0f;

    // --- Teclas blancas ---
    for (int i = 0; i < 7; ++i)
    {
        const auto nota = blancas[i];
        auto tecla = juce::Rectangle<float> (teclado.getX() + anchoBlanca * (float) i, teclado.getY(), anchoBlanca, teclado.getHeight())
                         .reduced (1.0f, 0.0f);

        const auto nivel = encendida[(size_t) nota];
        g.setColour (tema::color::boton.interpolatedWith (tema::color::enEscala, nivel));
        g.fillRoundedRectangle (tecla, 3.0f);

        if (nivel < 1.0f)
        {
            g.setColour (tema::color::bordeBoton.withAlpha (0.6f * (1.0f - nivel)));
            g.drawRoundedRectangle (tecla.reduced (0.5f), 3.0f, 1.0f);
        }

        dibujarPuntoTonica (g, tecla, tonica[(size_t) nota]);
    }

    // --- Teclas negras ---
    const auto anchoNegra = anchoBlanca * 0.6f;
    const auto altoNegra = teclado.getHeight() * 0.62f;

    for (int i = 0; i < 5; ++i)
    {
        const auto nota = negras[i];
        const auto x = teclado.getX() + anchoBlanca * posicionNegra[i] - anchoNegra * 0.5f;
        const auto tecla = juce::Rectangle<float> (x, teclado.getY() - 1.0f, anchoNegra, altoNegra + 1.0f);

        // Hueco oscuro alrededor para separar la negra de las blancas.
        g.setColour (tema::color::fondo);
        g.fillRoundedRectangle (tecla.expanded (1.5f, 0.0f).withTrimmedTop (-1.0f).withBottom (tecla.getBottom() + 1.5f), 3.0f);

        const auto nivel = encendida[(size_t) nota];
        g.setColour (tema::color::teclaNegra.interpolatedWith (tema::color::enEscala, nivel));
        g.fillRoundedRectangle (tecla, 2.5f);

        if (nivel < 1.0f)
        {
            g.setColour (tema::color::bordeTeclaNegra.withAlpha (1.0f - nivel));
            g.drawRoundedRectangle (tecla.reduced (0.5f), 2.5f, 1.0f);
        }

        dibujarPuntoTonica (g, tecla.withTrimmedBottom (-4.0f), tonica[(size_t) nota]);
    }

    // --- Leyenda ---
    columna.removeFromTop (10.0f);
    g.setColour (tema::color::textoSecundario);
    g.setFont (tema::inter (10.0f));
    g.drawText (juce::String (juce::CharPointer_UTF8 ("\xe2\x97\x8f t\xc3\xb3nica \xc2\xb7 teclas apagadas: fuera de escala")),
                columna.removeFromTop (16.0f), juce::Justification::centred, false);
}
