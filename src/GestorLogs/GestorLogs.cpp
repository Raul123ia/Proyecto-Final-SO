#include "GestorLogs.h"

// Constructor
GestorLogs::GestorLogs() {
}

// ================= RAM =================

void GestorLogs::logValidarMemoria(uint32_t mb) {
    logsRAM.push_back("Validando " + std::to_string(mb) + " MB de RAM");
}

void GestorLogs::logReservarMemoria(uint32_t pid, uint32_t mb) {
    logsRAM.push_back("PID " + std::to_string(pid) +
                      " reserva " + std::to_string(mb) + " MB");
}

void GestorLogs::logAsignarMemoria(uint32_t pid, uint32_t mb) {
    logsRAM.push_back("PID " + std::to_string(pid) +
                      " asigna " + std::to_string(mb) + " MB");
}

void GestorLogs::logLiberarMemoria(uint32_t pid, uint32_t mb) {
    logsRAM.push_back("PID " + std::to_string(pid) +
                      " libera " + std::to_string(mb) + " MB");
}

// ================= CPU =================

void GestorLogs::logAsignarCPU(uint32_t pid) {
    logsCPU.push_back("CPU asignada al PID " + std::to_string(pid));
}

void GestorLogs::logLiberarCPU() {
    logsCPU.push_back("CPU liberada");
}

// ================= FINALIZAR =================

std::vector<std::string> GestorLogs::exportarLogsRAM() const {
    return logsRAM;
}

std::vector<std::string> GestorLogs::exportarLogsCPU() const {
    return logsCPU;
}