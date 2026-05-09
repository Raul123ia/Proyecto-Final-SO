#include "../core/GestorLogs/GestorLogs.h"

// Constructor
GestorLogs::GestorLogs() {
}

// ================= UTILIDAD =================
void GestorLogs::anotarEvento(std::vector<std::string>& contenedor, const std::string& mensaje) {
    contenedor.push_back(mensaje);
    std::cout << mensaje << std::endl;
}

// ================= HISTORIAL =================
std::vector<std::string> GestorLogs::exportarHistorialLogs() const {
    return historialLogs;
}

// ================= RAM =================
void GestorLogs::logValidarDisponibilidadMemoria(uint32_t mb) {
    anotarEvento(logsRAM, "Validando " + std::to_string(mb) + " MB de RAM");
}
void GestorLogs::logReservarMemoria(uint32_t pid, uint32_t mb) {
    anotarEvento(logsRAM, "PID " + std::to_string(pid) +
                          " reserva " + std::to_string(mb) + " MB");
}
void GestorLogs::logAsignarMemoria(uint32_t pid, uint32_t mb) {
    anotarEvento(logsRAM, "PID " + std::to_string(pid) +
                          " asigna " + std::to_string(mb) + " MB");
}
void GestorLogs::logLiberarMemoria(uint32_t pid, uint32_t mb) {
    anotarEvento(logsRAM, "PID " + std::to_string(pid) +
                          " libera " + std::to_string(mb) + " MB");
}
void GestorLogs::logFinalizarMemoria(uint32_t pid) {
    anotarEvento(logsRAM, "PID " + std::to_string(pid) + " finaliza memoria");
}
std::vector<std::string> GestorLogs::exportarLogsRAM() {
    historialLogs.insert(historialLogs.end(), logsRAM.begin(), logsRAM.end());
    return logsRAM;
}

// ================= CPU =================
void GestorLogs::logAsignarCPU(uint32_t pid) {
    anotarEvento(logsCPU, "CPU asignada al PID " + std::to_string(pid));
}
void GestorLogs::logLiberarCPU() {
    anotarEvento(logsCPU, "CPU liberada");
}
void GestorLogs::logFinalizarCPU(uint32_t pid) {
    anotarEvento(logsCPU, "CPU finalizada por PID " + std::to_string(pid));
}
std::vector<std::string> GestorLogs::exportarLogsCPU() {
    historialLogs.insert(historialLogs.end(), logsCPU.begin(), logsCPU.end());
    return logsCPU;
}