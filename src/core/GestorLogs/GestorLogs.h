#ifndef GESTOR_LOGS_H
#define GESTOR_LOGS_H

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>

class GestorLogs {
private:
    std::vector<std::string> logsRAM;
    std::vector<std::string> logsCPU;
    std::vector<std::string> historialLogs;

public:
    GestorLogs();

    // ================= UTILIDAD =================
    void anotarEvento(const std::string& mensaje);

    // ================= HISTORIAL =================
    std::vector<std::string> exportarHistorialLogs() const;

    // ================= RAM =================
    std::vector<std::string> exportarLogsRAM();

    void logValidarDisponibilidadMemoria(uint32_t mb);
    void logAsignarMemoria(uint32_t pid, uint32_t mb);
    void logLiberarMemoria(uint32_t pid, uint32_t mb);
    void logInsuficienteMemoria(uint32_t requerido, uint32_t disponible);

    // ================= CPU =================
    std::vector<std::string> exportarLogsCPU();

    void logAsignarCPU(uint32_t pid);
    void logLiberarCPU();
};

#endif