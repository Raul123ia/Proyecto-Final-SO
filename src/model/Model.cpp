#include "Model.h"
#include <algorithm>

namespace sim {

std::atomic<uint32_t> Proceso::CONTADOR_PID{0};

Model::Model() = default;

const std::vector<Proceso>& Model::getProcesses() const {
    return procesos_;
}

void Model::iniciar(TipoAlgoritmo algoritmo, int quantum) {
    std::lock_guard<std::mutex> lk(mutex_);
    algoritmo_ = algoritmo;
    quantum_ = quantum;
}

void Model::crearProceso(const std::string& nombre, uint64_t rafaga, int prioridad, uint32_t memoria) {
    std::lock_guard<std::mutex> lk(mutex_);
    Proceso p;
    p.nombre = nombre;
    p.rafaga_restante = rafaga;
    p.prioridad = prioridad;
    p.memoria_asignada = memoria;
    p.estado = EstadoProceso::LISTO;
    procesos_.push_back(std::move(p));
}

void Model::ejecutarPasoSiguiente() {
    std::lock_guard<std::mutex> lk(mutex_);
    // Implementación mínima: si hay un proceso LISTO, ponerlo EJECUTANDO y decrementar rafaga
    auto it = std::find_if(procesos_.begin(), procesos_.end(), [](const Proceso& p){ return p.estado == EstadoProceso::LISTO; });
    if (it != procesos_.end()) {
        it->estado = EstadoProceso::EJECUTANDO;
        if (it->rafaga_restante > 0) it->rafaga_restante -= 1;
        if (it->rafaga_restante == 0) it->estado = EstadoProceso::TERMINADO;
    }
}

void Model::forzarSalidaProceso(uint32_t pid, CausaTerminacion causa) {
    std::lock_guard<std::mutex> lk(mutex_);
    auto it = std::find_if(procesos_.begin(), procesos_.end(), [pid](const Proceso& p){ return p.pid == pid; });
    if (it != procesos_.end()) {
        it->estado = EstadoProceso::TERMINADO;
        // En una implementación completa registrar causa en logs
    }
}

void Model::cambiarEstadoSuspension(uint32_t pid, bool suspender) {
    std::lock_guard<std::mutex> lk(mutex_);
    auto it = std::find_if(procesos_.begin(), procesos_.end(), [pid](const Proceso& p){ return p.pid == pid; });
    if (it != procesos_.end()) {
        if (suspender) it->estado = EstadoProceso::ESPERANDO; else it->estado = EstadoProceso::LISTO;
    }
}

} // namespace sim

