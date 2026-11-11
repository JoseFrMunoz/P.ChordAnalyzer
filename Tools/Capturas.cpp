/*
    Genera capturas PNG de la interfaz con resultados de ejemplo, sin plugin
    ni audio. Sirve para revisar el diseño y para las imágenes del README.

    Uso:  PChordAnalyzerCapturas <carpeta de salida>
*/
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Source/UI/Interfaz.h"

namespace
{
    pca::KeyResult ejemplo (int tonica, bool menor, float confianza)
    {
        pca::KeyResult r;
        r.hayEscala = true;
        r.tonica = tonica;
        r.menor = menor;
        r.confianza = confianza;
        r.silencio = false;
        return r;
    }

    void guardar (Interfaz& ui, const juce::File& carpeta, const juce::String& nombre)
    {
        const auto imagen = ui.createComponentSnapshot (ui.getLocalBounds(), true, 2.0f);
        const auto archivo = carpeta.getChildFile (nombre + ".png");
        archivo.deleteFile();

        juce::FileOutputStream salida (archivo);
        juce::PNGImageFormat().writeImageToStream (imagen, salida);
        std::cout << "Guardada " << archivo.getFullPathName() << std::endl;
    }
}

int main (int argc, char* argv[])
{
    const juce::ScopedJuceInitialiser_GUI gui;

    const auto carpeta = juce::File::getCurrentWorkingDirectory().getChildFile (argc > 1 ? argv[1] : "capturas");
    carpeta.createDirectory();

    Interfaz ui;

    ui.vistas.setResultado ({});
    guardar (ui, carpeta, "01-escuchando");

    ui.vistas.setResultado (ejemplo (9, true, 0.87f));
    guardar (ui, carpeta, "02-la-menor");

    ui.vistas.setResultado (ejemplo (6, true, 0.64f));
    guardar (ui, carpeta, "03-fa-sostenido-menor");

    ui.vistas.setResultado (ejemplo (3, false, 0.92f));
    ui.cabecera.botonCongelar.setToggleState (true, juce::dontSendNotification);
    guardar (ui, carpeta, "04-mi-bemol-mayor-congelado");

    ui.cabecera.botonCongelar.setToggleState (false, juce::dontSendNotification);
    ui.cabecera.botonBypass.setToggleState (true, juce::dontSendNotification);
    ui.cabecera.setBypass (true);
    ui.vistas.setAtenuada (true, false);
    guardar (ui, carpeta, "05-bypass");

    return 0;
}
