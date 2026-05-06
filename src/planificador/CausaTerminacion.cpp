#include "GestorRecursos.h"
#include "CausaTerminacion.h"

GestorRecursos::GestorRecursos() : memoria_usada(0), cpus_en_uso(0) {
}

// ================= MEMORIA =================

// VALIDAR
CausaTerminacion GestorRecursos::validarDisponibilidadMemoria(uint32_t mb) {

    if (mb == 0) return CausaTerminacion::USUARIO;

    if (memoria_usada >= MAX_MEMORIA) {
        return CausaTerminacion::INTERBLOQUEO;
    }

    if (memoria_usada + mb > MAX_MEMORIA) {
        return CausaTerminacion::ERROR;
    }

    return CausaTerminacion::NORMAL;
}

// RESERVAR (SOLO EJECUTA, NO VALIDA)
CausaTerminacion GestorRecursos::reservarMemoria(uint32_t pid, uint32_t mb) {

    memoria_usada += mb;
    mapa_memoria[pid] = mb;

    return CausaTerminacion::NORMAL;
}

// ASIGNAR (CONTROLADOR PRINCIPAL)
CausaTerminacion GestorRecursos::asignarMemoria(uint32_t pid, uint32_t mb) {

    CausaTerminacion estado = validarDisponibilidadMemoria(mb);

    if (estado != CausaTerminacion::NORMAL) {
        return estado;
    }

    return reservarMemoria(pid, mb);
}

// LIBERAR
CausaTerminacion GestorRecursos::liberarMemoria(uint32_t pid, uint32_t& mb_liberados) {

    if (mapa_memoria.find(pid) == mapa_memoria.end()) {
        mb_liberados = 0;
        return CausaTerminacion::ERROR;
    }

    mb_liberados = mapa_memoria[pid];
    memoria_usada -= mb_liberados;
    mapa_memoria.erase(pid);

    return CausaTerminacion::NORMAL;
}

// ================= CPU =================

// ASIGNAR CPU
CausaTerminacion GestorRecursos::asignarCPU() {

    if (cpus_en_uso >= MAX_CPUS) {
        return CausaTerminacion::INTERBLOQUEO;
    }

    cpus_en_uso++;
    return CausaTerminacion::NORMAL;
}

// LIBERAR CPU
CausaTerminacion GestorRecursos::liberarCPU() {

    if (cpus_en_uso <= 0) {
        return CausaTerminacion::ERROR;
    }

    cpus_en_uso--;
    return CausaTerminacion::NORMAL;
}

// ================= FINALIZAR =================

CausaTerminacion GestorRecursos::finalizarMemoria(uint32_t pid, uint32_t& mb_liberados) {
    return liberarMemoria(pid, mb_liberados);
}

CausaTerminacion GestorRecursos::finalizarCPU() {
    return liberarCPU();
}