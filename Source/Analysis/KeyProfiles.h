#pragma once

#include <array>

namespace pca
{
    /**
        Perfiles tonales para mayor y menor, empezando por la tónica.

        Elegidos: Krumhansl-Kessler (los que usa el detector por defecto).

        Por qué: probamos también los de Temperley (2007, corpus Kostka-Payne),
        que suelen recomendarse para música clásica. Con loops típicos de
        electrónica / hip-hop (pocos acordes repetidos, bajo marcando la raíz,
        progresiones i-VI-VII que comparten notas con la relativa mayor),
        Temperley confunde a menudo la escala menor con su relativa mayor,
        porque da casi el mismo peso a la tónica de ambas. Krumhansl-Kessler
        pesa más la tercera menor y el sexto grado en los perfiles menores y
        separa mejor el par mayor/relativa.

        En los tests sintéticos (24 escalas × 2 progresiones, Tests/) Krumhansl-
        Kessler acierta 48/48 y Temperley 39/48. El test "compararPerfiles"
        vigila que esta elección siga siendo la buena si se cambia el motor.
    */
    namespace perfiles
    {
        constexpr std::array<float, 12> temperleyMayor { 0.748f, 0.060f, 0.488f, 0.082f, 0.670f, 0.460f,
                                                         0.096f, 0.715f, 0.104f, 0.366f, 0.057f, 0.400f };
        constexpr std::array<float, 12> temperleyMenor { 0.712f, 0.084f, 0.474f, 0.618f, 0.049f, 0.460f,
                                                         0.105f, 0.747f, 0.404f, 0.067f, 0.133f, 0.330f };

        constexpr std::array<float, 12> krumhanslMayor { 6.35f, 2.23f, 3.48f, 2.33f, 4.38f, 4.09f,
                                                         2.52f, 5.19f, 2.39f, 3.66f, 2.29f, 2.88f };
        constexpr std::array<float, 12> krumhanslMenor { 6.33f, 2.68f, 3.52f, 5.38f, 2.60f, 3.53f,
                                                         2.54f, 4.75f, 3.98f, 2.69f, 3.34f, 3.17f };
    }
}
