#ifndef GESTOR_RECURSOS_H
#define GESTOR_RECURSOS_H
// vfcfdgh
#include <cstdint>
#include <map>
#include <set>

class GestorLogs;

class GestorRecursos {
private:
    uint32_t max_memoria;
    int max_cpus;
    uint32_t memoria_usada;
    std::map<uint32_t, uint32_t> mapa_memoria;
    std::set<uint32_t> pids_con_cpu;
    GestorLogs* logs;

public:

    // =============== ==VALORES POR DEFECTO =================
    GestorRecursos(GestorLogs* logs_instance = nullptr, uint32_t max_mem = 4096, int cpus = 1);

    // ================= MEMORIA =================
    bool validarDisponibilidadMemoria(uint32_t mb) const;
    bool asignarMemoria(uint32_t pid, uint32_t mb);
    uint32_t liberarMemoria(uint32_t pid);

    // ================= CPU =================
    bool asignarCPU(uint32_t pid);
    void liberarCPU(uint32_t pid);

    // ================= GETTERS =================
    uint32_t obtenerMemoriaUsada() const;
    int obtenerCPUsEnUso() const;
    uint32_t obtenerMemoriaMaxima() const;
};

#endif // GESTOR_RECURSOS_H