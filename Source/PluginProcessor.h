#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "Analysis/AnalysisEngine.h"

/**
    Procesador de P.ChordAnalyzer.

    El audio pasa intacto (paso directo) y la latencia es 0. En cada bloque
    solo se copia la mezcla mono al FIFO del motor de análisis, que trabaja en
    su propio hilo.
*/
class PChordAnalyzerProcessor final : public juce::AudioProcessor
{
public:
    PChordAnalyzerProcessor();
    ~PChordAnalyzerProcessor() override;

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

    /** Motor de análisis (la UI lee de aquí el resultado). */
    pca::AnalysisEngine& getMotor() noexcept { return motor; }

private:
    pca::AnalysisEngine motor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PChordAnalyzerProcessor)
};
