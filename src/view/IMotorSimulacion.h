#pragma once

#include <string>
#include <cstdint>

namespace sim {

// Interfaz abstracta que define el contrato entre la Vista y el Motor
class IMotorSimulacion {
public:
    virtual ~IMotorSimulacion() = default;

    // Iniciar la simulación con algoritmo y quantum
    virtual void iniciar(const std::string& algoritmo, int quantum) = 0;

    // Crear un nuevo proceso
    virtual void crearProceso(const std::string& nombre, uint64_t rafaga, int prioridad, uint32_t memoria) = 0;

    // Ejecutar el siguiente paso de simulación
    virtual void ejecutarPasoSiguiente() = 0;

    // Forzar salida de un proceso
    virtual void forzarSalidaProceso(uint32_t pid, const std::string& causa) = 0;

    // Cambiar estado de suspensión de un proceso
    virtual void cambiarEstadoSuspension(uint32_t pid, bool suspender) = 0;

    // Obtener información de estado (para actualizar el dashboard)
    virtual float obtenerUsoCPU() const = 0;
    virtual float obtenerUsoMemoria() const = 0;
    virtual uint32_t obtenerTotalProcesos() const = 0;
};

} // namespace sim

