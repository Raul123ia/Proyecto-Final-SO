#include "GestorRecursos.h"

GestorRecursos::GestorRecursos() : memoria_usada(0), cpus_en_uso(0) {
}

// ================= MEMORIA =================

bool GestorRecursos::validarDisponibilidad(uint32_t mb) {
    return (memoria_usada + mb <= MAX_MEMORIA);
}

void GestorRecursos::reservar(uint32_t pid, uint32_t mb) {
    if (validarDisponibilidad(mb)) {
        memoria_usada += mb;
        mapa_memoria[pid] = mb;
    }
}

void GestorRecursos::liberar(uint32_t pid) {
    if (mapa_memoria.find(pid) != mapa_memoria.end()) {
        memoria_usada -= mapa_memoria[pid];
        mapa_memoria.erase(pid);
    }
}

bool GestorRecursos::asignarMemoria(uint32_t pid, uint32_t mb) {
    if (!validarDisponibilidad(mb)) {
        return false;
    }
    reservar(pid, mb);
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
    cpus_en_uso = 0;
}

// ================= FINALIZAR =================

void GestorRecursos::finalizar(uint32_t pid) {
    liberar(pid);     // 🔁 Regresa la RAM del proceso
    liberarCPU();     // 🔁 Libera la CPU
}