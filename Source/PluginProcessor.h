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
    void processBlockBypassed (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;
    using AudioProcessor::processBlockBypassed;

    /** El host ve este parámetro como el bypass del plugin. */
    juce::AudioProcessorParameter* getBypassParameter() const override { return parametroBypass; }

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

    /** Parámetros del plugin (de momento, solo el bypass). */
    juce::AudioProcessorValueTreeState parametros;

    /** Congelar: estado de la UI (no es parámetro del host). Lo guarda el procesador
        para que no se pierda al cerrar y abrir la ventana. */
    std::atomic<bool> congelado { false };

    /** Vista abierta (0 = teclado, 1 = círculo, 2 = cromagrama). Se conserva
        al cerrar y abrir la ventana; en la fase 5 se guarda con el proyecto. */
    std::atomic<int> vistaActual { 1 };

    static constexpr const char* idBypass = "bypass";

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout crearParametros();

    pca::AnalysisEngine motor;
    juce::AudioParameterBool* parametroBypass = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PChordAnalyzerProcessor)
};
