#include "../core/GestorRecursos.h"

GestorRecursos::GestorRecursos(uint32_t memoriaTotal, uint32_t cpusTotales)
    : memoria(memoriaTotal), cpu(cpusTotales) {} // Inicializa los gestores de memoria y CPU

bool GestorRecursos::crearProceso(uint32_t pid, uint32_t memoriaReq) {
    // Delega el manejo de errores al llamador, no imprime a consola
    return memoria.asignar(pid, memoriaReq); // Intenta asignar memoria al proceso
}

bool GestorRecursos::terminarProceso(uint32_t pid) {
    // Si el proceso muere, primero lo sacamos de la CPU forzosamente
    cpu.liberarCPU(pid); // Libera la CPU asociada al proceso

    // Luego liberamos su memoria
    return memoria.liberar(pid); // Libera la memoria del proceso
}

bool GestorRecursos::asignarCPU(uint32_t pid) {
    // Validar que el proceso realmente exista (tenga memoria) antes de darle CPU
    // Nota: Esto asume que todo proceso válido debe tener memoria asignada.
    if (memoria.obtenerUsada() > 0) { // Verifica que exista al menos memoria usada
        return cpu.asignarCPU(pid); // Intenta asignar CPU al proceso
    }
    return false; // No se asigna CPU si no hay procesos válidos
}

void GestorRecursos::liberarCPU(uint32_t pid) {
    cpu.liberarCPU(pid); // Libera la CPU del proceso indicado
}

void GestorRecursos::ejecutarCiclo(uint64_t ciclos) {
    cpu.registrarCiclos(ciclos); // Registra ciclos de ejecución en la CPU
}

void GestorRecursos::mostrarEstado() const {
    memoria.mostrarEstado(); // Muestra el estado actual de la memoria
    cpu.mostrarEstado(); // Muestra el estado actual de la CPU
}