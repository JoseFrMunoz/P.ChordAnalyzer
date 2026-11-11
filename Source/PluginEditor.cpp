#include "PluginEditor.h"
#include "UI/Tema.h"

PChordAnalyzerEditor::PChordAnalyzerEditor (PChordAnalyzerProcessor& p)
    : AudioProcessorEditor (&p), procesador (p)
{
    addAndMakeVisible (interfaz);

    // --- Vista abierta: se recuerda en el procesador ---
    interfaz.setVista (procesador.vistaActual.load(), false);
    interfaz.onCambioVista = [this] (int indice) { procesador.vistaActual = indice; };

    // --- Congelar: la UI deja de actualizarse; el análisis sigue por detrás ---
    cabecera.botonCongelar.setToggleState (procesador.congelado.load(), juce::dontSendNotification);
    cabecera.botonCongelar.onClick = [this]
    {
        procesador.congelado = cabecera.botonCongelar.getToggleState();
        if (! procesador.congelado.load())
            leerResultado();   // al reanudar, mostrar el estado actual
    };

    // --- Bypass: parámetro del plugin ---
    auto* parametro = procesador.parametros.getParameter (PChordAnalyzerProcessor::idBypass);
    enlaceBypass = std::make_unique<juce::ParameterAttachment> (*parametro, [this] (float valor)
    {
        aplicarBypass (valor >= 0.5f, true);
    });

    cabecera.botonBypass.onClick = [this]
    {
        enlaceBypass->setValueAsCompleteGesture (cabecera.botonBypass.getToggleState() ? 1.0f : 0.0f);
    };

    // Estado inicial sin animación.
    aplicarBypass (parametro->getValue() >= 0.5f, false);

    // Tamaño fijo, no redimensionable.
    setResizable (false, false);
    setSize (tema::medida::anchoVentana, tema::medida::altoVentana);

    // Primer resultado aunque esté congelado (al abrir la ventana hay que ver algo).
    vistas.setResultado (procesador.getMotor().getResult());
    ultimaVersion = procesador.getMotor().getVersion();

    // Como mucho 30 lecturas por segundo; solo se repinta si algo cambia.
    startTimerHz (30);
}

PChordAnalyzerEditor::~PChordAnalyzerEditor()
{
    stopTimer();
}

void PChordAnalyzerEditor::aplicarBypass (bool activo, bool animar)
{
    cabecera.botonBypass.setToggleState (activo, juce::dontSendNotification);
    cabecera.setBypass (activo);
    vistas.setAtenuada (activo, animar);
}

void PChordAnalyzerEditor::timerCallback()
{
    if (procesador.congelado.load())
        return;

    leerResultado();
}

void PChordAnalyzerEditor::leerResultado()
{
    const auto version = procesador.getMotor().getVersion();
    if (version == ultimaVersion)
        return;

    ultimaVersion = version;
    vistas.setResultado (procesador.getMotor().getResult());
}

void PChordAnalyzerEditor::resized()
{
    interfaz.setBounds (getLocalBounds());
}
