#include "GestorMemoria.h" // Incluye la definición de la clase GestorMemoria
#include <iostream> // Para uso de std::cout

GestorMemoria::GestorMemoria(uint32_t memoriaTotal)
    : memoria_total(memoriaTotal), memoria_usada(0) {} // Inicializa memoria total y usada en 0

bool GestorMemoria::hayMemoriaDisponible(uint32_t memoria) const {
    std::lock_guard<std::mutex> lock(mtx); // Bloquea el acceso concurrente
    // Prevención de overflow en la suma
    if (memoria_total - memoria_usada < memoria) {
        return false; // No hay suficiente memoria disponible
    }
    return true; // Hay memoria suficiente
}

bool GestorMemoria::asignar(uint32_t pid, uint32_t memoria) {
    if (memoria == 0) return false; // Petición inválida (no se puede asignar 0 memoria)

    std::lock_guard<std::mutex> lock(mtx); // Protege el acceso concurrente

    // Evitar asignar memoria a un proceso que ya existe
    if (tabla_memoria.find(pid) != tabla_memoria.end()) {
        return false; // El proceso ya tiene memoria asignada
    }

    if (memoria_total - memoria_usada < memoria) {
        return false; // Memoria insuficiente
    }

    tabla_memoria.emplace(pid, memoria); // Inserta el PID con la memoria asignada
    memoria_usada += memoria; // Actualiza la memoria usada

    return true; // Asignación exitosa
}

bool GestorMemoria::liberar(uint32_t pid) {
    std::lock_guard<std::mutex> lock(mtx); // Bloquea acceso concurrente

    auto it = tabla_memoria.find(pid); // Busca el proceso en la tabla
    if (it == tabla_memoria.end()) {
        return false; // El proceso no existe, evita double free o errores
    }

    // Prevención de underflow en caso de corrupción de datos
    if (memoria_usada >= it->second) {
        memoria_usada -= it->second; // Resta la memoria asignada al proceso
    } else {
        memoria_usada = 0; // Fallback seguro si ocurre inconsistencia
    }

    tabla_memoria.erase(it); // Elimina el proceso de la tabla
    return true; // Liberación exitosa
}

uint32_t GestorMemoria::obtenerDisponible() const {
    std::lock_guard<std::mutex> lock(mtx); // Protege acceso concurrente
    return memoria_total - memoria_usada; // Retorna memoria disponible
}

uint32_t GestorMemoria::obtenerUsada() const {
    std::lock_guard<std::mutex> lock(mtx); // Bloquea acceso
    return memoria_usada; // Retorna memoria usada
}

void GestorMemoria::mostrarEstado() const {
    std::lock_guard<std::mutex> lock(mtx); // Bloquea acceso para consistencia
    std::cout << "\n--- ESTADO DE MEMORIA ---\n"; // Encabezado
    std::cout << "Total: " << memoria_total << " MB\n"; // Memoria total
    std::cout << "Usada: " << memoria_usada << " MB\n"; // Memoria usada
    std::cout << "Disponible: " << (memoria_total - memoria_usada) << " MB\n"; // Memoria libre

    for (const auto& p : tabla_memoria) { // Recorre todos los procesos registrados
        std::cout << "  PID " << p.first << " -> " << p.second << " MB\n"; // Muestra PID y memoria asignada
    }
}