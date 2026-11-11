#include "PluginProcessor.h"
#include "PluginEditor.h"

PChordAnalyzerProcessor::PChordAnalyzerProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Entrada", juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Salida",  juce::AudioChannelSet::stereo(), true)),
      parametros (*this, nullptr, "PARAMETROS", crearParametros())
{
    parametroBypass = dynamic_cast<juce::AudioParameterBool*> (parametros.getParameter (idBypass));
    jassert (parametroBypass != nullptr);
}

juce::AudioProcessorValueTreeState::ParameterLayout PChordAnalyzerProcessor::crearParametros()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { idBypass, 1 }, "Bypass", false));
    return layout;
}

PChordAnalyzerProcessor::~PChordAnalyzerProcessor()
{
    motor.stop();
}

void PChordAnalyzerProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{
    // Solo analizamos: no añadimos ningún retardo al audio.
    setLatencySamples (0);

    // Reserva la memoria del análisis y arranca su hilo (aquí sí se puede reservar).
    motor.prepare (sampleRate);
}

void PChordAnalyzerProcessor::releaseResources()
{
    motor.stop();
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
    const auto canalesEntrada = getTotalNumInputChannels();

    // Bypass: el audio sigue pasando intacto pero el análisis se detiene.
    motor.setActivo (! parametroBypass->get());

    // Copiamos la mezcla mono al FIFO del análisis antes de tocar nada.
    motor.pushAudio (buffer, canalesEntrada);

    // Standalone: la entrada se escucha pero no se reenvía a los altavoces
    // (evita acoples). En el plugin, paso directo intacto.
    if (wrapperType == wrapperType_Standalone)
    {
        buffer.clear();
        return;
    }

    for (auto canal = canalesEntrada; canal < getTotalNumOutputChannels(); ++canal)
        buffer.clear (canal, 0, buffer.getNumSamples());
}

void PChordAnalyzerProcessor::processBlockBypassed (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    // Bypass del host (si no usa nuestro parámetro): mismo comportamiento.
    motor.setActivo (false);

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
