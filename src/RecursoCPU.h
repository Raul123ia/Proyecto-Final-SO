#pragma once // Evita múltiples inclusiones del mismo archivo de cabecera

#include <cstdint> // Tipos de enteros de tamaño fijo (uint32_t, uint64_t)
#include <unordered_set> // Contenedor hash para almacenar PIDs sin duplicados
#include <mutex> // Para manejo de concurrencia (bloqueo de recursos compartidos)

class RecursoCPU {
private:
    uint32_t cpus_totales; // Número total de CPUs disponibles en el sistema

    // Ahora registramos QUÉ procesos tienen la CPU para poder hacer preempción/liberación real
    std::unordered_set<uint32_t> pids_en_cpu; // Conjunto de PIDs que actualmente ocupan CPU

    uint64_t ciclos_totales; // Contador total de ciclos de CPU utilizados

    mutable std::mutex mtx; // Mutex para proteger acceso concurrente a los datos (mutable permite usarlo en métodos const)

public:
    explicit RecursoCPU(uint32_t total); // Constructor que inicializa el total de CPUs disponibles

    [[nodiscard]] bool hayCPUDisponible() const; // Indica si hay al menos una CPU libre

    // Requieren el PID para saber a quién se le asigna/libera
    [[nodiscard]] bool asignarCPU(uint32_t pid); // Intenta asignar una CPU a un proceso (PID)

    [[nodiscard]] bool liberarCPU(uint32_t pid); // Libera la CPU ocupada por el proceso (PID)

    void registrarCiclos(uint64_t ciclos); // Suma ciclos de CPU al contador total

    uint32_t obtenerCPUsDisponibles() const; // Devuelve la cantidad de CPUs libres

    uint64_t obtenerCiclos() const; // Devuelve el total de ciclos registrados

    void mostrarEstado() const; // Muestra el estado actual del recurso CPU (uso, disponibles, etc.)
};