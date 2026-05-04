#pragma once

#include "../model/Model.h"
#include "../view/IMotorSimulacion.h"

namespace sim {

class Controller : public IMotorSimulacion {
public:
    explicit Controller(Model& model);

    // Métodos de IMotorSimulacion (virtuales)
    void iniciar(const std::string& algoritmo, int quantum) override;
    void crearProceso(const std::string& nombre, uint64_t rafaga, int prioridad, uint32_t memoria) override;
    void ejecutarPasoSiguiente() override;
    void forzarSalidaProceso(uint32_t pid, const std::string& causa) override;
    void cambiarEstadoSuspension(uint32_t pid, bool suspender) override;
    float obtenerUsoCPU() const override;
    float obtenerUsoMemoria() const override;
    uint32_t obtenerTotalProcesos() const override;

    const Model& model() const;

private:
    Model& model_;
};

} // namespace sim

