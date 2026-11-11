#include "KeyNames.h"

namespace pca::nombres
{
    namespace
    {
        juce::String utf8 (const char* texto) { return juce::String (juce::CharPointer_UTF8 (texto)); }

        // Ortografía habitual de cada tonalidad (la que tiene menos alteraciones).
        const char* const espanolMayor[12] { "Do", "Re\xe2\x99\xad", "Re", "Mi\xe2\x99\xad", "Mi", "Fa",
                                             "Fa\xe2\x99\xaf", "Sol", "La\xe2\x99\xad", "La", "Si\xe2\x99\xad", "Si" };
        const char* const espanolMenor[12] { "Do", "Do\xe2\x99\xaf", "Re", "Mi\xe2\x99\xad", "Mi", "Fa",
                                             "Fa\xe2\x99\xaf", "Sol", "Sol\xe2\x99\xaf", "La", "Si\xe2\x99\xad", "Si" };
        const char* const inglesMayor[12]  { "C", "D\xe2\x99\xad", "D", "E\xe2\x99\xad", "E", "F",
                                             "F\xe2\x99\xaf", "G", "A\xe2\x99\xad", "A", "B\xe2\x99\xad", "B" };
        const char* const inglesMenor[12]  { "C", "C\xe2\x99\xaf", "D", "E\xe2\x99\xad", "E", "F",
                                             "F\xe2\x99\xaf", "G", "G\xe2\x99\xaf", "A", "B\xe2\x99\xad", "B" };

        const char* const notasSostenidos[12] { "C", "C\xe2\x99\xaf", "D", "D\xe2\x99\xaf", "E", "F",
                                                "F\xe2\x99\xaf", "G", "G\xe2\x99\xaf", "A", "A\xe2\x99\xaf", "B" };
        const char* const notasBemoles[12]    { "C", "D\xe2\x99\xad", "D", "E\xe2\x99\xad", "E", "F",
                                                "G\xe2\x99\xad", "G", "A\xe2\x99\xad", "A", "B\xe2\x99\xad", "B" };

        int normalizar (int claseNota) { return ((claseNota % 12) + 12) % 12; }
    }

    juce::String nombreEspanol (int tonica, bool menor)
    {
        const auto t = normalizar (tonica);
        return utf8 (menor ? espanolMenor[t] : espanolMayor[t]) + (menor ? " menor" : " mayor");
    }

    juce::String cifrado (int tonica, bool menor)
    {
        const auto t = normalizar (tonica);
        return utf8 (menor ? inglesMenor[t] : inglesMayor[t]) + (menor ? "m" : "");
    }

    juce::String nota (int claseNota, bool conBemoles)
    {
        const auto n = normalizar (claseNota);
        return utf8 (conBemoles ? notasBemoles[n] : notasSostenidos[n]);
    }

    bool escalaConBemoles (int tonica, bool menor)
    {
        // Tónica de la mayor equivalente: las escalas con bemoles son F, B♭, E♭, A♭ y D♭ mayor.
        const auto mayor = menor ? normalizar (tonica + 3) : normalizar (tonica);
        return mayor == 5 || mayor == 10 || mayor == 3 || mayor == 8 || mayor == 1;
    }
}
