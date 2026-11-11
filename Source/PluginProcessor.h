#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

/**
    Procesador de P.ChordAnalyzer.

    Fase 1: el audio pasa intacto (paso directo) y la latencia es 0.
    En fases siguientes, aquí se copiará la mezcla mono a un FIFO sin bloqueos
    para que el hilo de análisis calcule la escala.
*/
class PChordAnalyzerProcessor final : public juce::AudioProcessor
{
public:
    PChordAnalyzerProcessor();
    ~PChordAnalyzerProcessor() override = default;

    // --- Ciclo de vida del audio ---
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    // --- Editor ---
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    // --- Información para el host ---
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    // --- Programas (no usamos presets de host) ---
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    // --- Estado (se completará en la fase 5) ---
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PChordAnalyzerProcessor)
};
