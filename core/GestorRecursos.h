#pragma once // Evita inclusiones múltiples del archivo de cabecera

#include "GestorMemoria.h"
#include "../src/RecursoCPU.h"
#include <cstdint> // Tipos enteros de tamaño fijo (uint32_t, uint64_t)

class GestorRecursos {
private:
    GestorMemoria memoria; // Objeto que gestiona la memoria del sistema
    RecursoCPU cpu; // Objeto que gestiona el uso de CPUs

public:
    GestorRecursos(uint32_t memoriaTotal, uint32_t cpusTotales); // Constructor que inicializa memoria y CPUs

    // Métodos seguros con valor de retorno obligatorio
    [[nodiscard]] bool crearProceso(uint32_t pid, uint32_t memoriaReq); // Crea un proceso asignándole memoria

    [[nodiscard]] bool terminarProceso(uint32_t pid); // Finaliza un proceso y libera sus recursos

    [[nodiscard]] bool asignarCPU(uint32_t pid); // Asigna CPU a un proceso

    void liberarCPU(uint32_t pid); // Libera la CPU ocupada por un proceso

    void ejecutarCiclo(uint64_t ciclos); // Simula la ejecución de ciclos de CPU

    void mostrarEstado() const; // Muestra el estado general del sistema (memoria y CPU)
};