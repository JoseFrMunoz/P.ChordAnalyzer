#pragma once

#include <array>
#include "../Analysis/KeyResult.h"

/** Utilidades de teoría para las vistas: qué notas pertenecen a la escala. */
namespace escala
{
    /** true si la clase de nota pertenece a la escala (mayor o menor natural). */
    inline bool contiene (const pca::KeyResult& r, int claseNota)
    {
        if (! r.hayEscala)
            return false;

        constexpr std::array<bool, 12> mayor { true, false, true, false, true, true, false, true, false, true, false, true };
        constexpr std::array<bool, 12> menor { true, false, true, true, false, true, false, true, true, false, true, false };

        const auto grado = (size_t) (((claseNota - r.tonica) % 12 + 12) % 12);
        return r.menor ? menor[grado] : mayor[grado];
    }

    inline bool esTonica (const pca::KeyResult& r, int claseNota)
    {
        return r.hayEscala && ((claseNota % 12) + 12) % 12 == r.tonica;
    }
}
