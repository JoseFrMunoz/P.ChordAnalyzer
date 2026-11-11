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
        // Cromagrama verosímil: notas de la escala fuertes, tónica y quinta más.
        const bool enMayor[12] { true, false, true, false, true, true, false, true, false, true, false, true };
        const bool enMenor[12] { true, false, true, true, false, true, false, true, true, false, true, false };
        const float extra[12]  { 1.0f, 0, 0.55f, 0.75f, 0.7f, 0.5f, 0, 0.85f, 0.6f, 0.45f, 0.5f, 0.4f };
        for (int n = 0; n < 12; ++n)
        {
            const auto grado = (n - tonica + 12) % 12;
            const auto dentro = menor ? enMenor[grado] : enMayor[grado];
            r.cromagrama[(size_t) n] = dentro ? extra[grado] : 0.08f + 0.04f * (float) (n % 3);
        }
        r.hayEscala = true;
        r.tonica = tonica;
        r.menor = menor;
        r.confianza = confianza;
        r.silencio = false;
        return r;
    }

    void guardar (Interfaz& ui, const juce::File& carpeta, const juce::String& nombre)
    {
        // Deja terminar fundidos, deslizamientos e interpolaciones antes de la foto.
        juce::MessageManager::getInstance()->runDispatchLoopUntil (800);

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

    // Las otras dos vistas (los resultados ya están cargados; el cromagrama
    // interpola, así que esperamos a que las barras lleguen a su sitio).
    ui.setVista (0, false);
    guardar (ui, carpeta, "02b-la-menor-teclado");
    ui.setVista (2, false);
    guardar (ui, carpeta, "02c-la-menor-cromagrama");
    ui.setVista (1, false);

    ui.vistas.setResultado (ejemplo (6, true, 0.64f));
    guardar (ui, carpeta, "03-fa-sostenido-menor");
    ui.setVista (0, false);
    guardar (ui, carpeta, "03b-fa-sostenido-menor-teclado");
    ui.setVista (2, false);
    guardar (ui, carpeta, "03c-fa-sostenido-menor-cromagrama");
    ui.setVista (1, false);

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
