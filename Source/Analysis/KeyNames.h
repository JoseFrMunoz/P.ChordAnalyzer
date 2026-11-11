#pragma once

#include <juce_core/juce_core.h>

namespace pca::nombres
{
    /** "La menor", "Do mayor"… */
    juce::String nombreEspanol (int tonica, bool menor);

    /** Cifrado inglés corto: "Am", "C", "F♯m"… */
    juce::String cifrado (int tonica, bool menor);

    /** Nombre en inglés de una nota (0 = C). Con sostenidos o bemoles según la escala. */
    juce::String nota (int claseNota, bool conBemoles);

    /** true si la escala se escribe con bemoles (Fa mayor, Re menor, Mi♭ mayor…). */
    bool escalaConBemoles (int tonica, bool menor);
}
