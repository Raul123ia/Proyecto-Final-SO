#include "../core/GestorLogs/GestorLogs.h"

// Constructor
GestorLogs::GestorLogs() {
}

// ================= UTILIDAD =================
void GestorLogs::anotarEvento (const std::string& mensaje) {
    historialLogs.push_back(mensaje); // Usa el atributo privado directamente
    std::cout << mensaje << std::endl;
}

// ================= HISTORIAL =================
std::vector<std::string> GestorLogs::exportarHistorialLogs() const {
    return historialLogs;
}

// ================= RAM =================
void GestorLogs::logValidarDisponibilidadMemoria(uint32_t mb) {
    std::string msg = "Validando " + std::to_string(mb) + " MB de RAM";
    logsRAM.push_back(msg);
    anotarEvento(msg);
}
void GestorLogs::logReservarMemoria(uint32_t pid, uint32_t mb) {
    std::string msg = "PID " + std::to_string(pid) + " reserva " + std::to_string(mb) + " MB";
    logsRAM.push_back(msg);
    anotarEvento(msg);
}

void GestorLogs::logAsignarMemoria(uint32_t pid, uint32_t mb) {
    std::string msg = "PID " + std::to_string(pid) + " asigna " + std::to_string(mb) + " MB";
    logsRAM.push_back(msg);
    anotarEvento(msg);
}

void GestorLogs::logLiberarMemoria(uint32_t pid, uint32_t mb) {
    std::string msg = "PID " + std::to_string(pid) + " libera " + std::to_string(mb) + " MB";
    logsRAM.push_back(msg);
    anotarEvento(msg);
}

void GestorLogs::logFinalizarMemoria(uint32_t pid) {
    std::string msg = "PID " + std::to_string(pid) + " finaliza memoria";
    logsRAM.push_back(msg);
    anotarEvento(msg);
}
std::vector<std::string> GestorLogs::exportarLogsRAM() {
    historialLogs.insert(historialLogs.end(), logsRAM.begin(), logsRAM.end());
    return logsRAM;
}

// ================= CPU =================

void GestorLogs::logAsignarCPU(uint32_t pid) {
    std::string msg = "CPU asignada al PID " + std::to_string(pid);
    logsCPU.push_back(msg);
    anotarEvento(msg);
}

void GestorLogs::logLiberarCPU() {
    std::string msg = "CPU liberada";
    logsCPU.push_back(msg);
    anotarEvento(msg);
}

void GestorLogs::logFinalizarCPU(uint32_t pid) {
    std::string msg = "CPU finalizada por PID " + std::to_string(pid);
    logsCPU.push_back(msg);
    anotarEvento(msg);
}


std::vector<std::string> GestorLogs::exportarLogsCPU() {
    historialLogs.insert(historialLogs.end(), logsCPU.begin(), logsCPU.end());
    return logsCPU;
}