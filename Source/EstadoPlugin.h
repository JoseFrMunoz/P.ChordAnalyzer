#pragma once

#include <juce_data_structures/juce_data_structures.h>

/**
    Estado que se guarda con el proyecto de FL Studio: vista abierta,
    congelar y bypass.

    Se separa del procesador para poder probar el guardado y la carga en
    los tests sin cargar el plugin.
*/
struct EstadoPlugin
{
    int  vista = 1;          ///< 0 = teclado, 1 = círculo (por defecto), 2 = cromagrama
    bool congelado = false;
    bool bypass = false;

    static constexpr int version = 1;

    /** Convierte el estado en XML (dentro de un MemoryBlock) para el host. */
    juce::MemoryBlock guardar() const;

    /**
        Lee un estado guardado. Si los datos están vacíos o dañados, o vienen
        de una versión desconocida, devuelve el estado por defecto (nunca falla).
        Los valores fuera de rango se corrigen.
    */
    static EstadoPlugin cargar (const void* datos, int tam);
};
