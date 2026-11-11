#pragma once

#include <array>
#include <cstddef>

namespace pca
{
    /** Número de clases de nota (C, C♯, D … B). */
    constexpr int numClasesNota = 12;

    using Cromagrama = std::array<float, numClasesNota>;

    /**
        Resultado del análisis que se entrega a la interfaz.
        Es un valor simple (sin punteros) para poder copiarlo entre hilos.
    */
    struct KeyResult
    {
        bool  hayEscala = false;   ///< false hasta que se detecta la primera escala
        int   tonica = 0;          ///< 0 = C, 1 = C♯ … 11 = B
        bool  menor = false;       ///< false = mayor, true = menor
        float confianza = 0.0f;    ///< 0 … 1 (la UI lo muestra como %)
        bool  silencio = true;     ///< true si la entrada está por debajo de ~-60 dBFS
        Cromagrama cromagrama {};  ///< energía actual de cada nota, normalizada a 0 … 1

        /** Tónica de la relativa (mayor ↔ menor). */
        int tonicaRelativa() const noexcept { return menor ? (tonica + 3) % 12 : (tonica + 9) % 12; }

        /** true si hay alguna diferencia visible respecto a otro resultado. */
        bool difiereDe (const KeyResult& otro) const noexcept
        {
            constexpr float tolerancia = 1.0e-4f;
            auto distinto = [] (float a, float b) { return (a > b ? a - b : b - a) > tolerancia; };

            if (hayEscala != otro.hayEscala || tonica != otro.tonica || menor != otro.menor
                || silencio != otro.silencio || distinto (confianza, otro.confianza))
                return true;

            for (std::size_t i = 0; i < cromagrama.size(); ++i)
                if (distinto (cromagrama[i], otro.cromagrama[i]))
                    return true;

            return false;
        }
    };
}
