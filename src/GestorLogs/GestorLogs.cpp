#include "../core/GestorLogs/GestorLogs.h"

// ================= CONSTRUCTOR =================
GestorLogs::GestorLogs() {
}

// ================= UTILIDAD =================
void GestorLogs::anotarEvento(const std::string& mensaje) {
    historialLogs.push_back(mensaje);
    std::cout << mensaje << std::endl;
}

// ================= HISTORIAL =================
std::vector<std::string> GestorLogs::exportarHistorialLogs() const {
    return historialLogs;
}

// ================= RAM =================
void GestorLogs::logValidarDisponibilidadMemoria(uint32_t mb) {
    // Filtrado para que no sature la vista (Tarea 1)
    // std::string mensaje ="Validando " + std::to_string(mb) + " MB de RAM";
    // logsRAM.push_back(mensaje);
    // anotarEvento(mensaje);
}

void GestorLogs::logAsignarMemoria(uint32_t pid, uint32_t mb) {
    std::string mensaje ="PID " + std::to_string(pid) +" asigna " + std::to_string(mb) + " MB";
    logsRAM.push_back(mensaje);
    anotarEvento(mensaje);
}

void GestorLogs::logLiberarMemoria(uint32_t pid, uint32_t mb) {
    std::string mensaje ="PID " + std::to_string(pid) +" libera " + std::to_string(mb) + " MB";
    logsRAM.push_back(mensaje);
    anotarEvento(mensaje);
}

void GestorLogs::logInsuficienteMemoria(uint32_t requerido, uint32_t disponible) {
    std::string mensaje = "Memoria insuficiente: se requieren "
        + std::to_string(requerido)
        + " MB pero solo hay disponibles "
        + std::to_string(disponible)
        + " MB";
    logsRAM.push_back(mensaje);
    anotarEvento(mensaje);
}

std::vector<std::string> GestorLogs::exportarLogsRAM() {
    return logsRAM;
}

// ================= CPU =================
void GestorLogs::logAsignarCPU(uint32_t pid) {
    std::string mensaje ="CPU asignada al PID " + std::to_string(pid);
    logsCPU.push_back(mensaje);
    anotarEvento(mensaje);
}

void GestorLogs::logLiberarCPU() {
    std::string mensaje = "CPU liberada";
    logsCPU.push_back(mensaje);
    anotarEvento(mensaje);
}

std::vector<std::string> GestorLogs::exportarLogsCPU() {
    return logsCPU;
}