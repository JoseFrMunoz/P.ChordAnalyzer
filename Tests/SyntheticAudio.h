#pragma once

#include <juce_core/juce_core.h>
#include <cmath>
#include <random>
#include <vector>

/**
    Generador de audio sintético para los tests: acordes y melodías con
    armónicos (parecido a un sinte o un piano eléctrico), con bajo opcional.
*/
namespace sintetico
{
    struct Evento
    {
        std::vector<int> notasMidi;  ///< notas que suenan a la vez
        double duracion;             ///< segundos
    };

    inline double frecuenciaMidi (int midi) { return 440.0 * std::pow (2.0, (midi - 69) / 12.0); }

    /** Renderiza una secuencia de eventos a mono. */
    inline std::vector<float> renderizar (const std::vector<Evento>& eventos, double fs,
                                          int numArmonicos = 6, float amplitud = 0.2f, unsigned semilla = 1)
    {
        std::vector<float> salida;
        std::mt19937 rng (semilla);
        std::uniform_real_distribution<double> fase (0.0, 2.0 * juce::MathConstants<double>::pi);

        for (const auto& e : eventos)
        {
            const auto n = (size_t) (e.duracion * fs);
            const auto inicio = salida.size();
            salida.resize (inicio + n, 0.0f);

            for (auto midi : e.notasMidi)
            {
                const auto f0 = frecuenciaMidi (midi);

                for (int h = 1; h <= numArmonicos; ++h)
                {
                    const auto f = f0 * h;
                    if (f > fs * 0.45) break;

                    const auto fi = fase (rng);
                    const auto a = amplitud / h / (double) e.notasMidi.size();

                    for (size_t i = 0; i < n; ++i)
                    {
                        const auto t = (double) i / fs;
                        // Envolvente: ataque de 10 ms y caída suave.
                        const auto env = std::min (1.0, t / 0.01) * std::exp (-1.5 * t / e.duracion);
                        salida[inicio + i] += (float) (a * env * std::sin (2.0 * juce::MathConstants<double>::pi * f * t + fi));
                    }
                }
            }
        }

        return salida;
    }

    /** Silencio de la duración indicada. */
    inline std::vector<float> silencio (double segundos, double fs)
    {
        return std::vector<float> ((size_t) (segundos * fs), 0.0f);
    }

    /** Triada (+ bajo una o dos octavas abajo) sobre una fundamental MIDI. */
    inline std::vector<int> triada (int raiz, bool menor, bool conBajo = true)
    {
        std::vector<int> notas { raiz, raiz + (menor ? 3 : 4), raiz + 7 };
        if (conBajo)
            notas.push_back (raiz - 24);
        return notas;
    }

    /** Repite una progresión hasta cubrir los segundos indicados. */
    inline std::vector<Evento> repetir (const std::vector<Evento>& progresion, double segundos)
    {
        std::vector<Evento> salida;
        double t = 0.0;
        while (t < segundos)
            for (const auto& e : progresion)
            {
                salida.push_back (e);
                t += e.duracion;
            }
        return salida;
    }

    /** Lleva una nota MIDI a la octava de C4 (60 … 71). */
    inline int octavaMedia (int claseNota) { return 60 + ((claseNota % 12) + 12) % 12; }
}
