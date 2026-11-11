#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "UI/Interfaz.h"

/**
    Ventana de P.ChordAnalyzer (340 × 330, no redimensionable).

    De arriba abajo: cabecera (36 px), zona de vistas (230 px) y selector de
    puntos (24 px, fase 4).

    Lee el resultado del motor 30 veces por segundo como máximo y solo
    repinta lo que cambia.
*/
class PChordAnalyzerEditor final : public juce::AudioProcessorEditor,
                                   private juce::Timer
{
public:
    explicit PChordAnalyzerEditor (PChordAnalyzerProcessor&);
    ~PChordAnalyzerEditor() override;

    void resized() override;

private:
    void timerCallback() override;
    void leerResultado();
    void aplicarBypass (bool activo, bool animar);

    PChordAnalyzerProcessor& procesador;

    Interfaz interfaz;
    Cabecera& cabecera = interfaz.cabecera;
    ZonaVistas& vistas = interfaz.vistas;

    /** Conecta el botón de bypass con el parámetro (incluida la automatización del host). */
    std::unique_ptr<juce::ParameterAttachment> enlaceBypass;

    juce::uint32 ultimaVersion = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PChordAnalyzerEditor)
};
