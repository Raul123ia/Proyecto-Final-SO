#ifndef GESTOR_RECURSOS_H
#define GESTOR_RECURSOS_H

#include <cstdint>
#include <string>
#include <map>

class GestorRecursos {
private:
    const uint32_t MAX_MEMORIA = 4096;
    const int MAX_CPUS = 1;

    uint32_t memoria_usada;
    int cpus_en_uso;

    std::map<uint32_t, uint32_t> mapa_memoria;

public:
    GestorRecursos();

    // --- MEMORIA ---
    bool validarDisponibilidadMemoria(uint32_t mb);
    void reservarMemoria(uint32_t pid, uint32_t mb);
    uint32_t liberarMemoria(uint32_t pid);
    bool asignarMemoria(uint32_t pid, uint32_t mb);

    // --- CPU ---
    bool asignarCPU();
    void liberarCPU();

    // --- FINALIZACIÓN ---
    uint32_t finalizarMemoria(uint32_t pid);
    void finalizarCPU();

};

#endif // GESTOR_RECURSOS_H