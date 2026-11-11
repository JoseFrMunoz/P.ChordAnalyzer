/*
    Aplicación Standalone (.exe) propia.

    Es la de JUCE con un cambio: la entrada de audio NO se silencia. JUCE la
    silencia por defecto para evitar acoples, pero aquí la Standalone existe
    precisamente para escuchar la entrada del sistema. Para que no haya
    acople, el procesador deja la salida en silencio cuando corre como
    Standalone (ver PluginProcessor::processBlock).
*/
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h>

namespace
{
    class AppStandalone final : public juce::JUCEApplication
    {
    public:
        AppStandalone()
        {
            juce::PropertiesFile::Options opciones;
            opciones.applicationName     = juce::CharPointer_UTF8 (JucePlugin_Name);
            opciones.filenameSuffix      = ".settings";
            opciones.osxLibrarySubFolder = "Application Support";
           #if JUCE_LINUX || JUCE_BSD
            opciones.folderName          = "~/.config";
           #endif
            propiedades.setStorageParameters (opciones);
        }

        const juce::String getApplicationName() override           { return juce::CharPointer_UTF8 (JucePlugin_Name); }
        const juce::String getApplicationVersion() override        { return JucePlugin_VersionString; }
        bool moreThanOneInstanceAllowed() override                 { return true; }
        void anotherInstanceStarted (const juce::String&) override {}

        void initialise (const juce::String&) override
        {
            auto holder = std::make_unique<juce::StandalonePluginHolder> (propiedades.getUserSettings(), false);

            // Escuchar siempre la entrada (ver comentario de arriba).
            holder->getMuteInputValue() = false;

            ventana = std::make_unique<juce::StandaloneFilterWindow> (getApplicationName(),
                                                                       juce::Colour (0xFF0E0E10),
                                                                       std::move (holder));
            ventana->setVisible (true);
        }

        void shutdown() override
        {
            ventana = nullptr;
            propiedades.saveIfNeeded();
        }

        void systemRequestedQuit() override
        {
            if (ventana != nullptr)
                ventana->getPluginHolder()->savePluginState();

            quit();
        }

    private:
        juce::ApplicationProperties propiedades;
        std::unique_ptr<juce::StandaloneFilterWindow> ventana;
    };
}

juce::JUCEApplicationBase* juce_CreateApplication();
juce::JUCEApplicationBase* juce_CreateApplication() { return new AppStandalone(); }
