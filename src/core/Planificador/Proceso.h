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

public:
    // Constructor
    Proceso(std::string _nombre, int _prioridad, uint64_t _rafaga, uint32_t _memoria);

    // Getters esenciales para la UI y el Planificador
    uint32_t obtenerPid() const;
    std::string obtenerNombre() const;
    EstadoProceso obtenerEstado() const;
    uint64_t obtenerRafagaRestante() const;
    int obtenerPrioridad() const;

    // Métodos de control
    bool actualizarEstado(EstadoProceso nuevo);
    void vincularRecurso(std::string nombre);
    
    // Novedad: Método para simular el paso del tiempo en el procesador
    void ejecutarUnTick(); 
};

#endif // PROCESO_H