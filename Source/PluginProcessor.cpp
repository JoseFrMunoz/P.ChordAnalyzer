#include "PluginProcessor.h"
#include "PluginEditor.h"

PChordAnalyzerProcessor::PChordAnalyzerProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Entrada", juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Salida",  juce::AudioChannelSet::stereo(), true))
{
}

void PChordAnalyzerProcessor::prepareToPlay (double /*sampleRate*/, int /*samplesPerBlock*/)
{
    // Solo analizamos: no añadimos ningún retardo al audio.
    setLatencySamples (0);
}

void PChordAnalyzerProcessor::releaseResources()
{
}

bool PChordAnalyzerProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Aceptamos mono o estéreo, siempre con el mismo formato en entrada y salida.
    const auto& salida = layouts.getMainOutputChannelSet();

    if (salida != juce::AudioChannelSet::mono() && salida != juce::AudioChannelSet::stereo())
        return false;

    return salida == layouts.getMainInputChannelSet();
}

void PChordAnalyzerProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals sinDenormales;

    // Paso directo: el audio de entrada ya está en el buffer y sale tal cual.
    // Solo limpiamos canales de salida que no tengan entrada correspondiente.
    for (auto canal = getTotalNumInputChannels(); canal < getTotalNumOutputChannels(); ++canal)
        buffer.clear (canal, 0, buffer.getNumSamples());
}

juce::AudioProcessorEditor* PChordAnalyzerProcessor::createEditor()
{
    return new PChordAnalyzerEditor (*this);
}

void PChordAnalyzerProcessor::getStateInformation (juce::MemoryBlock& /*destData*/)
{
    // Fase 5: guardar vista actual, congelar y bypass.
}

void PChordAnalyzerProcessor::setStateInformation (const void* /*data*/, int /*sizeInBytes*/)
{
    // Fase 5: restaurar vista actual, congelar y bypass.
}

// Punto de entrada que usa JUCE para crear el plugin.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PChordAnalyzerProcessor();
}
