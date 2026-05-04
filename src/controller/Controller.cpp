#include "Controller.h"
#include <algorithm>

namespace sim {

Controller::Controller(Model& model)
    : model_(model) {}

void Controller::iniciar(const std::string& algoritmo, int quantum) {
    TipoAlgoritmo algo = TipoAlgoritmo::FCFS;
    if (algoritmo == "SJF") algo = TipoAlgoritmo::SJF;
    else if (algoritmo == "ROUND ROBIN") algo = TipoAlgoritmo::ROUND_ROBIN;
    else if (algoritmo == "PRIORIDADES") algo = TipoAlgoritmo::PRIORIDADES;

    model_.iniciar(algo, quantum);
}

void Controller::crearProceso(const std::string& nombre, uint64_t rafaga, int prioridad, uint32_t memoria) {
    model_.crearProceso(nombre, rafaga, prioridad, memoria);
}

void Controller::ejecutarPasoSiguiente() {
    model_.ejecutarPasoSiguiente();
}

void Controller::forzarSalidaProceso(uint32_t pid, const std::string& causa) {
    CausaTerminacion c = CausaTerminacion::NORMAL;
    if (causa == "ERROR") c = CausaTerminacion::ERROR_;
    else if (causa == "INTERBLOQUEO") c = CausaTerminacion::INTERBLOQUEO;
    else if (causa == "USUARIO") c = CausaTerminacion::USUARIO;

    model_.forzarSalidaProceso(pid, c);
}

void Controller::cambiarEstadoSuspension(uint32_t pid, bool suspender) {
    model_.cambiarEstadoSuspension(pid, suspender);
}

float Controller::obtenerUsoCPU() const {
    const auto& procs = model_.getProcesses();
    uint32_t ejecutando = 0;
    for (const auto& p : procs) {
        if (p.estado == EstadoProceso::EJECUTANDO) ejecutando++;
    }
    return ejecutando > 0 ? 50.0f + (ejecutando % 50) : 0.0f;
}

float Controller::obtenerUsoMemoria() const {
    const auto& procs = model_.getProcesses();
    uint32_t total = 0;
    for (const auto& p : procs) {
        if (p.estado != EstadoProceso::TERMINADO) total += p.memoria_asignada;
    }
    return (float)total;
}

uint32_t Controller::obtenerTotalProcesos() const {
    return model_.getProcesses().size();
}

const Model& Controller::model() const {
    return model_;
}

} // namespace sim
