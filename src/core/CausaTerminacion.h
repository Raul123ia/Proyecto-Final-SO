#ifndef GESTOR_RECURSOS_H
#define GESTOR_RECURSOS_H

#include <cstdint>
#include <map>
#include "CausaTerminacion.h"

class GestorRecursos {
private:
    const uint32_t MAX_MEMORIA = 4096;
    const int MAX_CPUS = 1;

    uint32_t memoria_usada;
    int cpus_en_uso;

    std::map<uint32_t, uint32_t> mapa_memoria; // PID -> MB

public:
    GestorRecursos();

    // ================= MEMORIA (interno) =================
    bool validarDisponibilidadMemoria(uint32_t mb);
    void reservarMemoria(uint32_t pid, uint32_t mb);
    uint32_t liberarMemoria(uint32_t pid, uint32_t& mb_liberados);

    // ================= CPU (interno) =================
    bool asignarCPU();
    void liberarCPU();

    // ================= FINALIZAR (alto nivel) =================
    CausaTerminacion asignarMemoria(uint32_t pid, uint32_t mb);
    CausaTerminacion asignarCPUProceso();
    CausaTerminacion finalizarMemoria(uint32_t pid, uint32_t& mb_liberados);
    CausaTerminacion finalizarCPU();
};

#endif