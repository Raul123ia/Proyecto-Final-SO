#include "RecursoCPU.h" // Incluye la definición de la clase RecursoCPU
#include <iostream> // Para entrada/salida estándar (cout)

RecursoCPU::RecursoCPU(uint32_t total)
    : cpus_totales(total), ciclos_totales(0) {} // Inicializa el total de CPUs y los ciclos en 0

bool RecursoCPU::hayCPUDisponible() const {
    std::lock_guard<std::mutex> lock(mtx); // Bloquea el mutex para acceso seguro en concurrencia
    return pids_en_cpu.size() < cpus_totales; // Retorna true si hay CPUs libres
}

bool RecursoCPU::asignarCPU(uint32_t pid) {
    std::lock_guard<std::mutex> lock(mtx); // Bloquea el acceso para evitar condiciones de carrera

    if (pids_en_cpu.size() >= cpus_totales) return false; // Si no hay CPUs disponibles, retorna false

    // Si el proceso ya tiene CPU, no consume un core extra
    auto resultado = pids_en_cpu.insert(pid); // Inserta el PID en el conjunto
    return resultado.second; // Devuelve true si se insertó (no existía), false si ya estaba
}

bool RecursoCPU::liberarCPU(uint32_t pid) {
    std::lock_guard<std::mutex> lock(mtx); // Bloquea el acceso concurrente
    return pids_en_cpu.erase(pid) > 0; // Elimina el PID y retorna true si existía
}

void RecursoCPU::registrarCiclos(uint64_t ciclos) {
    std::lock_guard<std::mutex> lock(mtx); // Protección de acceso concurrente
    // Solo registrar ciclos si realmente hay CPUs trabajando
    if (!pids_en_cpu.empty()) {
        ciclos_totales += ciclos; // Acumula los ciclos al total
    }
}

uint32_t RecursoCPU::obtenerCPUsDisponibles() const {
    std::lock_guard<std::mutex> lock(mtx); // Bloquea para lectura segura
    return cpus_totales - static_cast<uint32_t>(pids_en_cpu.size()); // Calcula CPUs libres
}

uint64_t RecursoCPU::obtenerCiclos() const {
    std::lock_guard<std::mutex> lock(mtx); // Protección de acceso
    return ciclos_totales; // Retorna el total de ciclos acumulados
}

void RecursoCPU::mostrarEstado() const {
    std::lock_guard<std::mutex> lock(mtx); // Bloquea el acceso para evitar inconsistencias
    std::cout << "\n--- ESTADO DE CPU ---\n"; // Encabezado del estado
    std::cout << "Cores Totales: " << cpus_totales << "\n"; // Muestra CPUs totales
    std::cout << "Cores Disponibles: " << (cpus_totales - pids_en_cpu.size()) << "\n"; // Muestra CPUs libres
    std::cout << "Ciclos Totales: " << ciclos_totales << "\n"; // Muestra ciclos acumulados

    if (!pids_en_cpu.empty()) { // Si hay procesos en ejecución
        std::cout << "PIDs en ejecución: ";
        for (uint32_t pid : pids_en_cpu) std::cout << pid << " "; // Imprime cada PID
        std::cout << "\n";
    }
}