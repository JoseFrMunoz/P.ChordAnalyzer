#include "PluginEditor.h"
#include "Analysis/KeyNames.h"

namespace
{
    // Medidas y colores de la especificación visual.
    constexpr int anchoVentana = 340;
    constexpr int altoVentana  = 330;
    constexpr float radioEsquinas = 6.0f;

    const juce::Colour colorFondo      { 0xFF0E0E10 };
    const juce::Colour colorBorde      { 0xFF26262B };
    const juce::Colour colorTexto      { 0xFFF5F5F7 };
    const juce::Colour colorSecundario { 0xFF8A8A92 };
}

PChordAnalyzerEditor::PChordAnalyzerEditor (PChordAnalyzerProcessor& p)
    : AudioProcessorEditor (&p), procesador (p)
{
    // Tamaño fijo, no redimensionable.
    setResizable (false, false);
    setSize (anchoVentana, altoVentana);

    resultado = procesador.getMotor().getResult();

    // La UI consulta el resultado 10 veces por segundo y solo repinta si cambia.
    startTimerHz (10);
}

PChordAnalyzerEditor::~PChordAnalyzerEditor()
{
    stopTimer();
}

void PChordAnalyzerEditor::timerCallback()
{
    const auto version = procesador.getMotor().getVersion();
    if (version == ultimaVersion)
        return;

    ultimaVersion = version;
    resultado = procesador.getMotor().getResult();
    repaint();
}

void PChordAnalyzerEditor::paint (juce::Graphics& g)
{
    // El host pinta detrás de las esquinas redondeadas: usamos el mismo
    // tono de fondo para que no se vean picos claros.
    g.fillAll (colorFondo);

    const auto area = getLocalBounds().toFloat().reduced (0.5f);
    g.setColour (colorBorde);
    g.drawRoundedRectangle (area, radioEsquinas, 1.0f);

    // --- Resultado provisional en texto (fase 2) ---
    auto zona = getLocalBounds().reduced (12);

    g.setColour (colorSecundario);
    g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
    g.drawText ("P.CHORDANALYZER", zona.removeFromTop (24), juce::Justification::centredLeft);

    zona.removeFromTop (70);

    if (! resultado.hayEscala)
    {
        g.setFont (juce::FontOptions (16.0f));
        g.drawText (juce::String (juce::CharPointer_UTF8 ("Escuchando\xe2\x80\xa6")),
                    zona.removeFromTop (30), juce::Justification::centred);
        return;
    }

    const auto nombre = pca::nombres::nombreEspanol (resultado.tonica, resultado.menor);
    const auto cifrado = pca::nombres::cifrado (resultado.tonica, resultado.menor);
    const auto relativa = pca::nombres::nombreEspanol (resultado.tonicaRelativa(), ! resultado.menor);
    const auto porcentaje = juce::String (juce::roundToInt (resultado.confianza * 100.0f)) + " %";

    g.setColour (colorTexto);
    g.setFont (juce::FontOptions (28.0f, juce::Font::bold));
    g.drawText (nombre, zona.removeFromTop (40), juce::Justification::centred);

    g.setColour (colorSecundario);
    g.setFont (juce::FontOptions (16.0f));
    g.drawText (cifrado + "  \xc2\xb7  " + porcentaje, zona.removeFromTop (26), juce::Justification::centred);
    g.drawText ("relativa: " + relativa, zona.removeFromTop (24), juce::Justification::centred);

    if (resultado.silencio)
        g.drawText ("(silencio: se mantiene la ultima escala)", zona.removeFromTop (24), juce::Justification::centred);
}

void PChordAnalyzerEditor::resized()
{
    // Fase 2: todavía no hay componentes que colocar.
}
