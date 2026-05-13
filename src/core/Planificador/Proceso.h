#ifndef PROCESO_H
#define PROCESO_H

#include <string>
#include <vector>
#include <atomic>
#include <cstdint>

enum class EstadoProceso {
    LISTO,
    EJECUTANDO,
    ESPERANDO,
    TERMINADO
};

enum class TipoProceso {
    NORMAL,
    PRODUCTOR,
    CONSUMIDOR
};

class Proceso {
private:
    static std::atomic_uint32_t CONTADOR_PID;
    uint32_t pid;
    std::string nombre;
    EstadoProceso estado;
    int prioridad;
    uint64_t rafaga_restante;
    uint32_t memoria_asignada;
    std::vector<std::string> recursos_fisicos;
    int contador_programa;
    TipoProceso tipo_proceso;

public:
    // Constructor
    Proceso(std::string _nombre, int _prioridad, uint64_t _rafaga, uint32_t _memoria, TipoProceso _tipo = TipoProceso::NORMAL);

    // Getters esenciales para la UI y el Planificador
    [[nodiscard]] uint32_t obtenerPid() const;
    [[nodiscard]] std::string obtenerNombre() const;
    [[nodiscard]] EstadoProceso obtenerEstado() const;
    [[nodiscard]] uint64_t obtenerRafagaRestante() const;
    [[nodiscard]] int obtenerPrioridad() const;
    [[nodiscard]] uint32_t obtenerMemoriaAsignada() const;

    int obtenerContadorPrograma() const;
    TipoProceso obtenerTipoProceso() const;
    void establecerContadorPrograma(int cp);
    // Métodos de control
    bool actualizarEstado(EstadoProceso nuevo);
    void vincularRecurso(std::string nombre);
    void actualizarMemoriaAsignada(uint32_t memoria);
    // Novedad: Método para simular el paso del tiempo en el procesador
    void ejecutarUnTick();
};


#endif // PROCESO_H