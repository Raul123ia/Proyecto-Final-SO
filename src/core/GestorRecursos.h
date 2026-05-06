#ifndef GESTOR_RECURSOS_H
#define GESTOR_RECURSOS_H

#include <cstdint>
#include <string>
#include <map>
#include <queue>

class GestorRecursos {
private:
    const uint32_t MAX_MEMORIA = 4096;
    const int MAX_CPUS = 1;

    uint32_t memoria_usada;
    int cpus_en_uso;

    std::map<uint32_t, uint32_t> mapa_memoria;
    std::map<std::string, std::queue<uint32_t>> procesos_bloqueados;

public:
    GestorRecursos();

    // --- MEMORIA ---
    bool validarDisponibilidad(uint32_t mb);
    void reservar(uint32_t pid, uint32_t mb);
    void liberar(uint32_t pid);
    bool asignarMemoria(uint32_t pid, uint32_t mb);

    // --- CPU ---
    bool asignarCPU();
    void liberarCPU();

    // --- FINALIZACIÓN (LO NUEVO IMPORTANTE) ---
    void finalizar(uint32_t pid);

};

#endif // GESTOR_RECURSOS_H