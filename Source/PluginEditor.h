#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

/**
    Ventana de P.ChordAnalyzer.

    Fase 1: ventana fija de 340 × 330 px con el fondo y el borde definitivos.
*/
class PChordAnalyzerEditor final : public juce::AudioProcessorEditor
{
public:
    explicit PChordAnalyzerEditor (PChordAnalyzerProcessor&);
    ~PChordAnalyzerEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PChordAnalyzerProcessor& procesador;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PChordAnalyzerEditor)
};
