/*
    Pasa un archivo de audio por el mismo motor de análisis del plugin y
    muestra, cada medio segundo, la escala detectada, la confianza, las
    mejores candidatas y el cromagrama. Sirve para depurar con audio real.

    Uso:  PChordAnalyzerAnalizar <archivo.wav|.aiff|.flac|.mp3>
*/
#include <juce_audio_formats/juce_audio_formats.h>
#include "../Source/Analysis/AnalysisCore.h"
#include "../Source/Analysis/KeyNames.h"
#include <algorithm>
#include <iostream>

int main (int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Uso: PChordAnalyzerAnalizar <archivo de audio>" << std::endl;
        return 1;
    }

    juce::AudioFormatManager formatos;
    formatos.registerBasicFormats();

    const juce::File archivo (juce::File::getCurrentWorkingDirectory().getChildFile (argv[1]));
    std::unique_ptr<juce::AudioFormatReader> lector (formatos.createReaderFor (archivo));
    if (lector == nullptr)
    {
        std::cerr << "No se puede leer " << archivo.getFullPathName() << std::endl;
        return 1;
    }

    const auto fs = lector->sampleRate;
    const auto total = (int) lector->lengthInSamples;
    std::cout << archivo.getFileName() << ": " << fs << " Hz, " << lector->numChannels << " canales, "
              << total / fs << " s" << std::endl;

    juce::AudioBuffer<float> audio ((int) lector->numChannels, total);
    lector->read (&audio, 0, total, 0, true, true);

    // Mezcla mono (L+R)/2, igual que el plugin.
    std::vector<float> mono ((size_t) total);
    for (int i = 0; i < total; ++i)
    {
        float s = 0.0f;
        for (int c = 0; c < audio.getNumChannels(); ++c)
            s += audio.getSample (c, i);
        mono[(size_t) i] = s / (float) audio.getNumChannels();
    }

    pca::AnalysisCore core;
    core.prepare (fs);

    const auto paso = (int) (fs * 0.5);
    for (int i = 0; i < total; i += paso)
    {
        core.pushSamples (mono.data() + i, std::min (paso, total - i));
        const auto r = core.getResult();
        const auto& c = core.getDetector().getCorrelaciones();

        std::array<int, 24> idx {};
        for (int k = 0; k < 24; ++k) idx[(size_t) k] = k;
        std::sort (idx.begin(), idx.end(), [&] (int a, int b) { return c[(size_t) a] > c[(size_t) b]; });

        juce::String linea;
        linea << juce::String ((double) (i + paso) / fs, 1).paddedLeft (' ', 5) << " s  ";
        linea << (r.hayEscala ? pca::nombres::cifrado (r.tonica, r.menor) : juce::String ("--")).paddedRight (' ', 4)
              << juce::String (juce::roundToInt (r.confianza * 100)).paddedLeft (' ', 3) << " %  |";
        for (int k = 0; k < 3; ++k)
            linea << " " << pca::nombres::cifrado (idx[(size_t) k] % 12, idx[(size_t) k] >= 12)
                  << "=" << juce::String (c[(size_t) idx[(size_t) k]], 2);
        linea << "  | croma:";
        for (int n = 0; n < 12; ++n)
            linea << " " << juce::String (juce::roundToInt (r.cromagrama[(size_t) n] * 9));
        std::cout << linea << std::endl;
    }

    return 0;
}
