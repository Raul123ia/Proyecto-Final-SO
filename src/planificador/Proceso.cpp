#include "core/Planificador/Proceso.h"
#include "core/Planificador/Planificador.h"
// Inicialización de la variable estática. El PID 0 está reservado.
std::atomic_uint32_t Proceso::CONTADOR_PID{1};

// Constructor usando listas de inicialización 
Proceso::Proceso(std::string _nombre, int _prioridad, uint64_t _rafaga, uint32_t _memoria)
    : pid(CONTADOR_PID++), nombre(_nombre), estado(EstadoProceso::LISTO), 
      prioridad(_prioridad), rafaga_restante(_rafaga), memoria_asignada(_memoria) {
}

// Getters
uint32_t Proceso::obtenerPid() const { return pid; }
std::string Proceso::obtenerNombre() const { return nombre; }
EstadoProceso Proceso::obtenerEstado() const { return estado; }
uint64_t Proceso::obtenerRafagaRestante() const { return rafaga_restante; }

// Métodos de control
bool Proceso::actualizarEstado(EstadoProceso nuevo) {
    // Protección básica: un proceso terminado ya no puede cambiar de estado
    if (estado == EstadoProceso::TERMINADO) {
        return false;
    }
    estado = nuevo;
    return true;
}

void Proceso::vincularRecurso(std::string nombre_recurso) {
    recursos_fisicos.push_back(nombre_recurso);
}

// El motor del PCB
void Proceso::ejecutarUnTick() {
    if (rafaga_restante > 0) {
        rafaga_restante--;
    }
}

int Proceso::obtenerPrioridad() const {
    return prioridad;
}