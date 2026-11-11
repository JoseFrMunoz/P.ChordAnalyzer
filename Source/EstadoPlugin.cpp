#include "EstadoPlugin.h"

namespace
{
    const juce::Identifier idRaiz      { "PChordAnalyzer" };
    const juce::Identifier idVersion   { "version" };
    const juce::Identifier idVista     { "vista" };
    const juce::Identifier idCongelado { "congelado" };
    const juce::Identifier idBypass    { "bypass" };
}

juce::MemoryBlock EstadoPlugin::guardar() const
{
    juce::ValueTree arbol (idRaiz);
    arbol.setProperty (idVersion, version, nullptr);
    arbol.setProperty (idVista, vista, nullptr);
    arbol.setProperty (idCongelado, congelado, nullptr);
    arbol.setProperty (idBypass, bypass, nullptr);

    juce::MemoryBlock bloque;
    juce::MemoryOutputStream salida (bloque, false);
    salida.writeString (arbol.toXmlString());   // XML legible, fácil de depurar
    salida.flush();
    return bloque;
}

EstadoPlugin EstadoPlugin::cargar (const void* datos, int tam)
{
    EstadoPlugin estado;

    if (datos == nullptr || tam <= 0)
        return estado;

    juce::MemoryInputStream entrada (datos, (size_t) tam, false);
    const auto xml = juce::parseXML (entrada.readString());
    if (xml == nullptr)
        return estado;

    const auto arbol = juce::ValueTree::fromXml (*xml);
    if (! arbol.hasType (idRaiz) || (int) arbol.getProperty (idVersion, 0) > version)
        return estado;

    estado.vista     = juce::jlimit (0, 2, (int) arbol.getProperty (idVista, estado.vista));
    estado.congelado = (bool) arbol.getProperty (idCongelado, estado.congelado);
    estado.bypass    = (bool) arbol.getProperty (idBypass, estado.bypass);
    return estado;
}
