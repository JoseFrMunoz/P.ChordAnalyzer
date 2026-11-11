#include "PluginEditor.h"

namespace
{
    // Medidas y colores de la especificación visual.
    constexpr int anchoVentana = 340;
    constexpr int altoVentana  = 330;
    constexpr float radioEsquinas = 6.0f;

    const juce::Colour colorFondo { 0xFF0E0E10 };
    const juce::Colour colorBorde { 0xFF26262B };
}

PChordAnalyzerEditor::PChordAnalyzerEditor (PChordAnalyzerProcessor& p)
    : AudioProcessorEditor (&p), procesador (p)
{
    // Tamaño fijo, no redimensionable.
    setResizable (false, false);
    setSize (anchoVentana, altoVentana);
}

void PChordAnalyzerEditor::paint (juce::Graphics& g)
{
    // El host pinta detrás de las esquinas redondeadas: usamos el mismo
    // tono de fondo para que no se vean picos claros.
    g.fillAll (colorFondo);

    const auto area = getLocalBounds().toFloat().reduced (0.5f);

    g.setColour (colorFondo);
    g.fillRoundedRectangle (area, radioEsquinas);

    g.setColour (colorBorde);
    g.drawRoundedRectangle (area, radioEsquinas, 1.0f);
}

void PChordAnalyzerEditor::resized()
{
    // Fase 1: todavía no hay componentes que colocar.
}
