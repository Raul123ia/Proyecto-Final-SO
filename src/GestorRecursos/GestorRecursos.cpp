#include "../core/GestorRecursos/GestorRecursos.h"
#include "../core/GestorLogs/GestorLogs.h"

GestorRecursos::GestorRecursos(GestorLogs* logs_instance) : memoria_usada(0), cpus_en_uso(0), logs(logs_instance) {
}

// ================= MEMORIA =================

bool GestorRecursos::validarDisponibilidadMemoria(uint32_t mb) const {
    if (logs) logs->logValidarDisponibilidadMemoria(mb);
    return (memoria_usada + mb <= MAX_MEMORIA);
}

void GestorRecursos::reservarMemoria(uint32_t pid, uint32_t mb) {
    if (validarDisponibilidadMemoria(mb)) {
        if (mapa_memoria.count(pid)) {
            memoria_usada -= mapa_memoria[pid];
        }
        memoria_usada += mb;
        mapa_memoria[pid] = mb;
        if (logs) logs->logReservarMemoria(pid, mb);
    }
}

uint32_t GestorRecursos::liberarMemoria(uint32_t pid) {
    if (mapa_memoria.find(pid) != mapa_memoria.end()) {
        uint32_t mb_liberados = mapa_memoria[pid];
        memoria_usada -= mb_liberados;
        mapa_memoria.erase(pid);
        if (logs) logs->logLiberarMemoria(pid, mb_liberados);
        return mb_liberados;
    }
    return 0;
}

bool GestorRecursos::asignarMemoria(uint32_t pid, uint32_t mb) {
    if (!validarDisponibilidadMemoria(mb)) {
        return false;
    }
    reservarMemoria(pid, mb);
    if (logs) logs->logAsignarMemoria(pid, mb);
    return true;
}

// ================= CPU =================

bool GestorRecursos::asignarCPU(uint32_t pid) {
    if (cpus_en_uso < MAX_CPUS) {
        cpus_en_uso++;
        if (logs) logs->logAsignarCPU(pid);
        return true;
    }
    return false;
}

void GestorRecursos::liberarCPU() {
    if (cpus_en_uso > 0) {
        cpus_en_uso--;
        if (logs) logs->logLiberarCPU();
    }
}

// ================= FINALIZAR =================

uint32_t GestorRecursos::finalizarMemoria(uint32_t pid) {
    return liberarMemoria(pid);
}

void GestorRecursos::finalizarCPU() {
    liberarCPU();
}