#pragma once // Evita inclusiones múltiples del mismo archivo de cabecera

#include <cstdint> // Tipos enteros de tamaño fijo (uint32_t, etc.)
#include <unordered_map> // Contenedor hash para mapear PID -> memoria asignada
#include <mutex> // Para control de concurrencia con mutex

class GestorMemoria {
private:
    uint32_t memoria_total; // Cantidad total de memoria disponible
    uint32_t memoria_usada; // Cantidad de memoria actualmente en uso

    std::unordered_map<uint32_t, uint32_t> tabla_memoria; // Almacena qué proceso (PID) tiene cuánta memoria

    // Mutex mutable para permitir thread-safety incluso en métodos const
    mutable std::mutex mtx; // Protege el acceso concurrente a los datos

public:
    explicit GestorMemoria(uint32_t memoriaTotal); // Constructor que inicializa la memoria total

    [[nodiscard]] bool hayMemoriaDisponible(uint32_t memoria) const; // Verifica si hay memoria suficiente disponible

    // nodiscard obliga al llamador a comprobar si la asignación/liberación fue exitosa
    [[nodiscard]] bool asignar(uint32_t pid, uint32_t memoria); // Asigna memoria a un proceso identificado por PID

    [[nodiscard]] bool liberar(uint32_t pid); // Libera la memoria asociada a un proceso

    uint32_t obtenerDisponible() const; // Devuelve la memoria disponible

    uint32_t obtenerUsada() const; // Devuelve la memoria utilizada

    void mostrarEstado() const; // Muestra el estado actual del sistema de memoria
};