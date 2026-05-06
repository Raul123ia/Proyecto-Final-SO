#include "GestorRecursos.h"

GestorRecursos::GestorRecursos() : memoria_usada(0), cpus_en_uso(0) {
}

// ================= MEMORIA =================

bool GestorRecursos::validarDisponibilidadMemoria(uint32_t mb) {
    return (memoria_usada + mb <= MAX_MEMORIA);
}

void GestorRecursos::reservarMemoria(uint32_t pid, uint32_t mb) {
    if (validarDisponibilidadMemoria(mb)) {
        memoria_usada += mb;
        mapa_memoria[pid] = mb;
    }
}

uint32_t GestorRecursos::liberarMemoria(uint32_t pid) {
    if (mapa_memoria.find(pid) != mapa_memoria.end()) {
        uint32_t mb_liberados = mapa_memoria[pid];
        memoria_usada -= mb_liberados;
        mapa_memoria.erase(pid);
        return mb_liberados;
    }
    return 0; // Retorna 0 si el PID no tenía memoria asignada
}

bool GestorRecursos::asignarMemoria(uint32_t pid, uint32_t mb) {
    if (!validarDisponibilidadMemoria(mb)) {
        return false;
    }
    reservarMemoria(pid, mb);
    return true;
}

// ================= CPU =================

bool GestorRecursos::asignarCPU() {
    if (cpus_en_uso < MAX_CPUS) {
        cpus_en_uso++;
        return true;
    }
    return false;
}

void GestorRecursos::liberarCPU() {
    if (cpus_en_uso > 0) {
        cpus_en_uso--;
    }
}

// ================= FINALIZAR =================

// Libera SOLO la RAM del proceso y retorna los MB liberados
uint32_t GestorRecursos::finalizarMemoria(uint32_t pid) {
    return liberarMemoria(pid);
}

// Libera SOLO la CPU
void GestorRecursos::finalizarCPU() {
    liberarCPU();
}