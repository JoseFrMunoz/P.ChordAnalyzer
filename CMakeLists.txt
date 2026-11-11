# P.ChordAnalyzer · proyecto CMake con JUCE 8
cmake_minimum_required(VERSION 3.22)

project(PChordAnalyzer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# En Windows usamos el runtime estático para que el .vst3 y el .exe
# no dependan de las DLL de Visual C++ instaladas en el PC.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# --- JUCE descargado automáticamente (no hace falta tenerlo instalado) ---
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

# --- Definición del plugin ---
juce_add_plugin(PChordAnalyzer
    # Nombre de los archivos: sin punto, porque el punto rompe el paso de
    # JUCE que genera el moduleinfo del VST3.
    PRODUCT_NAME            "PChordAnalyzer"
    # Nombre que muestra FL Studio en su lista de plugins.
    PLUGIN_NAME             "P.ChordAnalyzer"
    COMPANY_NAME            "P.P"
    BUNDLE_ID               "com.pp.chordanalyzer"
    PLUGIN_MANUFACTURER_CODE PPpp   # Código de fabricante común a todos los plugins de P.P (no cambiar)
    PLUGIN_CODE             PCan    # Código único de este plugin
    FORMATS                 VST3 Standalone
    IS_SYNTH                FALSE
    NEEDS_MIDI_INPUT        FALSE
    NEEDS_MIDI_OUTPUT       FALSE
    IS_MIDI_EFFECT          FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES         Fx Analyzer
)

# Interfaz: se compila en el plugin y en la herramienta de capturas.
set(PCA_FUENTES_UI
    Source/UI/Tema.cpp
    Source/UI/BotonIcono.cpp
    Source/UI/Cabecera.cpp
    Source/UI/VistaCirculo.cpp
    Source/UI/ZonaVistas.cpp
    Source/UI/Interfaz.cpp
)

# Motor de análisis: se compila tanto en el plugin como en los tests.
set(PCA_FUENTES_ANALISIS
    Source/Analysis/ChromaExtractor.cpp
    Source/Analysis/KeyDetector.cpp
    Source/Analysis/AnalysisCore.cpp
    Source/Analysis/AnalysisEngine.cpp
    Source/Analysis/KeyNames.cpp
)

target_sources(PChordAnalyzer PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/StandaloneApp.cpp
    ${PCA_FUENTES_UI}
    ${PCA_FUENTES_ANALISIS}
)

# Tipografías embebidas (licencia OFL, ver Resources/Fonts/README.md).
juce_add_binary_data(PChordAnalyzerFuentes
    HEADER_NAME BinaryData.h
    NAMESPACE   BinaryData
    SOURCES
        Resources/Fonts/Inter-Regular.ttf
        Resources/Fonts/Inter-SemiBold.ttf
        Resources/Fonts/Inter-Bold.ttf
        Resources/Fonts/JetBrainsMono-Regular.ttf
)

target_compile_definitions(PChordAnalyzer PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=1   # Source/StandaloneApp.cpp
)

target_link_libraries(PChordAnalyzer
    PRIVATE
        PChordAnalyzerFuentes
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# --- Tests unitarios (se ejecutan con CTest en GitHub Actions) ---
option(PCA_TESTS "Compilar los tests unitarios" ON)

if(PCA_TESTS)
    enable_testing()

    juce_add_console_app(PChordAnalyzerTests
        PRODUCT_NAME "PChordAnalyzerTests"
    )

    target_sources(PChordAnalyzerTests PRIVATE
        Tests/TestMain.cpp
        Tests/KeyDetectionTests.cpp
        ${PCA_FUENTES_ANALISIS}
    )

    target_compile_definitions(PChordAnalyzerTests PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
    )

    target_link_libraries(PChordAnalyzerTests
        PRIVATE
            juce::juce_core
            juce::juce_audio_basics
            juce::juce_dsp
        PUBLIC
            juce::juce_recommended_config_flags
            juce::juce_recommended_warning_flags
    )

    add_test(NAME PChordAnalyzerTests COMMAND PChordAnalyzerTests)
endif()

# --- Herramienta de capturas de la interfaz (opcional, no se usa en CI) ---
option(PCA_CAPTURAS "Compilar la herramienta que genera capturas PNG de la interfaz" OFF)

if(PCA_CAPTURAS)
    juce_add_console_app(PChordAnalyzerCapturas PRODUCT_NAME "PChordAnalyzerCapturas")

    target_sources(PChordAnalyzerCapturas PRIVATE
        Tools/Capturas.cpp
        ${PCA_FUENTES_UI}
        Source/Analysis/KeyNames.cpp
    )

    target_compile_definitions(PChordAnalyzerCapturas PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)

    target_link_libraries(PChordAnalyzerCapturas
        PRIVATE
            PChordAnalyzerFuentes
            juce::juce_gui_basics
        PUBLIC
            juce::juce_recommended_config_flags
            juce::juce_recommended_warning_flags
    )
endif()
