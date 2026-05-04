#pragma once

#include <atomic>
#include <cstdint>
#include <string>
#include <vector>
#include <mutex>

namespace sim {

enum class EstadoProceso { LISTO, EJECUTANDO, ESPERANDO, TERMINADO };

enum class TipoAlgoritmo { FCFS, SJF, ROUND_ROBIN, PRIORIDADES };

enum class CausaTerminacion { NORMAL, ERROR_, INTERBLOQUEO, USUARIO };

struct Proceso {
    static std::atomic<uint32_t> CONTADOR_PID;
    uint32_t pid = 0;
    std::string nombre;
    EstadoProceso estado = EstadoProceso::LISTO;
    int prioridad = 0;
    uint64_t rafaga_restante = 0;
    uint32_t memoria_asignada = 0;
    std::vector<std::string> recursos_fisicos;

    Proceso() {
        pid = ++CONTADOR_PID;
    }

    uint32_t obtenerPid() const { return pid; }
    void actualizarEstado(EstadoProceso nuevo) { estado = nuevo; }
    void vincularRecurso(const std::string& nombreRecurso) { recursos_fisicos.push_back(nombreRecurso); }
};

class Model {
public:
    Model();

    // Acceso inmutable al estado actual
    const std::vector<Proceso>& getProcesses() const;

    // Operaciones según diagrama (métodos mínimos implementados)
    void iniciar(TipoAlgoritmo algoritmo, int quantum);
    void crearProceso(const std::string& nombre, uint64_t rafaga, int prioridad, uint32_t memoria);
    void ejecutarPasoSiguiente();
    void forzarSalidaProceso(uint32_t pid, CausaTerminacion causa);
    void cambiarEstadoSuspension(uint32_t pid, bool suspender);

private:
    std::vector<Proceso> procesos_;
    mutable std::mutex mutex_;
    TipoAlgoritmo algoritmo_ = TipoAlgoritmo::FCFS;
    int quantum_ = 0;
};

} // namespace sim

