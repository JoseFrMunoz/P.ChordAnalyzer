#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/**
    Colores, medidas y tipografías de la interfaz (especificación visual).
    Todo lo visual sale de aquí para poder retocarlo en un solo sitio.
*/
namespace tema
{
    namespace color
    {
        const juce::Colour fondo            { 0xFF0E0E10 };
        const juce::Colour borde            { 0xFF26262B };
        const juce::Colour textoPrincipal   { 0xFFF5F5F7 };
        const juce::Colour textoSecundario  { 0xFF8A8A92 };
        const juce::Colour textoTerciario   { 0xFFC8C8CE };
        const juce::Colour enEscala         { 0xFFEDEDF0 };
        const juce::Colour tonica           { 0xFFFFFFFF };
        const juce::Colour fueraDeEscala    { 0xFF2E2E34 };
        const juce::Colour teclaNegra       { 0xFF08080A };
        const juce::Colour bordeTeclaNegra  { 0xFF3A3A40 };
        const juce::Colour boton            { 0xFF1C1C20 };
        const juce::Colour bordeBoton       { 0xFF3A3A40 };
        const juce::Colour anillo           { 0xFF26262B };
    }

    namespace medida
    {
        constexpr int anchoVentana  = 340;
        constexpr int altoVentana   = 330;
        constexpr int padding       = 12;
        constexpr int altoCabecera  = 36;
        constexpr int altoVistas    = 230;
        constexpr int altoPuntos    = 24;
        constexpr int ladoBoton     = 36;
        constexpr float radioVentana = 6.0f;
        constexpr float radioBoton   = 6.0f;
    }

    /** Pesos de Inter disponibles. */
    enum class Peso { normal, seminegrita, negrita };

    /** Inter (textos). Los símbolos ♯/♭ salen de JetBrains Mono como respaldo. */
    juce::FontOptions inter (float tamPx, Peso peso = Peso::normal);

    /** JetBrains Mono (notas y porcentajes). */
    juce::FontOptions mono (float tamPx);
}
