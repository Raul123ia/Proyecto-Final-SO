#include "../core/GestorRecursos/GestorRecursos.h"
#include "../core/GestorLogs/GestorLogs.h"

// ================= CONSTRUCTOR =================
GestorRecursos::GestorRecursos(GestorLogs* logs_instance, uint32_t max_mem, int cpus)
    : max_memoria(max_mem), max_cpus(cpus), memoria_usada(0), logs(logs_instance) {
}

// ================= MEMORIA =================
bool GestorRecursos::validarDisponibilidadMemoria(uint32_t mb) const {
    if (logs) logs->logValidarDisponibilidadMemoria(mb);
    return (memoria_usada + mb <= max_memoria);
}

bool GestorRecursos::asignarMemoria(uint32_t pid, uint32_t mb) {
    if (!validarDisponibilidadMemoria(mb)) {
        return false;
    }

    if (mapa_memoria.count(pid)) {
        memoria_usada -= mapa_memoria[pid];
    }

    memoria_usada += mb;
    mapa_memoria[pid] = mb;

    if (logs) logs->logAsignarMemoria(pid, mb);
    return true;
}

uint32_t GestorRecursos::liberarMemoria(uint32_t pid) {
    auto it = mapa_memoria.find(pid);
    if (it != mapa_memoria.end()) {
        uint32_t mb_liberados = it->second;
        memoria_usada -= mb_liberados;
        mapa_memoria.erase(it);

        if (logs) logs->logLiberarMemoria(pid, mb_liberados);
        return mb_liberados;
    }
    return 0;
}

// ================= CPU =================
bool GestorRecursos::asignarCPU(uint32_t pid) {
    if (pids_con_cpu.count(pid)) return true;

    if (pids_con_cpu.size() < max_cpus) {
        pids_con_cpu.insert(pid);
        if (logs) logs->logAsignarCPU(pid);
        return true;
    }
    return false;
}

void GestorRecursos::liberarCPU(uint32_t pid) {
    if (pids_con_cpu.erase(pid) > 0) {
        if (logs) logs->logLiberarCPU();
    }
}

// ================= GETTERS =================
uint32_t GestorRecursos::obtenerMemoriaUsada() const {
    return memoria_usada;
}

int GestorRecursos::obtenerCPUsEnUso() const {
    return pids_con_cpu.size();
}
uint32_t GestorRecursos::obtenerMemoriaMaxima() const {
    return max_memoria;
}

