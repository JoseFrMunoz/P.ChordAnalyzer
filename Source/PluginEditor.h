#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

/**
    Ventana de P.ChordAnalyzer.

    Fase 2: ventana fija de 340 × 330 px que muestra el resultado del análisis
    en texto. La interfaz definitiva llega en las fases 3 y 4.
*/
class PChordAnalyzerEditor final : public juce::AudioProcessorEditor,
                                   private juce::Timer
{
public:
    explicit PChordAnalyzerEditor (PChordAnalyzerProcessor&);
    ~PChordAnalyzerEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    PChordAnalyzerProcessor& procesador;

    pca::KeyResult resultado;
    juce::uint32 ultimaVersion = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PChordAnalyzerEditor)
};
