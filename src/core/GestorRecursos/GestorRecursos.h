#ifndef GESTOR_RECURSOS_H
#define GESTOR_RECURSOS_H

#include <cstdint>
#include <map>

class GestorLogs;

class GestorRecursos {
private:
    const uint32_t MAX_MEMORIA = 4096;
    const int MAX_CPUS = 1;

    uint32_t memoria_usada;
    int cpus_en_uso;

    std::map<uint32_t, uint32_t> mapa_memoria;

    GestorLogs* logs;

public:
    GestorRecursos(GestorLogs* logs_instance = nullptr);

    // ================= MEMORIA =================
    bool validarDisponibilidadMemoria(uint32_t mb) const;
    void reservarMemoria(uint32_t pid, uint32_t mb);
    uint32_t liberarMemoria(uint32_t pid);
    bool asignarMemoria(uint32_t pid, uint32_t mb);

    // ================= CPU =================
    bool asignarCPU(uint32_t pid); // Se agregó el pid para que coincida con el log
    void liberarCPU();

    // ================= FINALIZAR =================
    uint32_t finalizarMemoria(uint32_t pid);
    void finalizarCPU();

};

#endif // GESTOR_RECURSOS_H