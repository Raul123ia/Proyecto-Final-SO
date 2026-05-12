#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H

#include <queue>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <stdexcept>
#include "Proceso.h"
#include "core/GestorComunicacion/GestorComunicacion.h" // Necesario para conocer el módulo de tu compañero

// Enum para identificar el servicio solicitado al Kernel
enum class TipoLlamada {
    WAIT_SEMAFORO,
    SIGNAL_SEMAFORO
};

enum class TipoAlgoritmo {
    FCFS,
    SJF,
    ROUND_ROBIN,
    PRIORIDADES
};

enum class CausaTerminacion {
    NORMAL,
    ERROR,
    INTERBLOQUEO,
    USUARIO
};

class Planificador {
private:
    TipoAlgoritmo algoritmo;
    int valor_quantum;
    int ticks_ejecutados_quantum; // Solo relevante para Round Robin
    // Novedad: El reloj del sistema (Tiempo Simulado)
    uint64_t reloj_global;
    
    // Novedad: La Tabla de Procesos Central (dueña de la memoria)
    std::unordered_map<uint32_t, Proceso> tabla_procesos;
    
    // Las colas ahora solo guardan el ID del proceso
    std::queue<uint32_t> cola_listos;
    std::queue<uint32_t> cola_suspendidos;
    std::vector<uint32_t> historial_terminados;
    
    // 0 significa que la CPU está inactiva (idle)
    uint32_t pid_en_ejecucion; 

public:
    Planificador(TipoAlgoritmo _algoritmo, int _quantum = 0);

    // Novedad: API Segura para tus compañeros de UI y Recursos
    // Devuelve una referencia de solo lectura al PCB
    const Proceso& obtenerDetallesProceso(uint32_t pid) const;

    // Novedad: El motor de la simulación. 
    // Llamar a este método avanza el reloj 1 unidad de tiempo.
    void avanzarTiempo();
    void ejecutarCPU(); // Simula el trabajo del procesador (resta ráfaga, etc.)
    void ejecutarCicloCompleto(); // Método aglomerador para correr todo el ciclo de un golpe (avanzar tiempo + despachador + CPU)
    // Métodos públicos de operación (ahora trabajan por copia o ID)
    // Recibe los datos y el Planificador construye el proceso internamente
    uint32_t crearYAsignarProceso(std::string nombre, int prioridad, uint64_t rafaga, uint32_t memoria, TipoProceso tipo = TipoProceso::NORMAL);
    
    void suspender(uint32_t pid);
    void reanudar(uint32_t pid);
    void finalizar(uint32_t pid, CausaTerminacion causa);
    void ejecutarDespachador();
    void actualizarContadorPrograma(uint32_t pid, int nuevo_contador_programa);
    // Getters para el estado general del sistema (útil para la UI)
    uint64_t obtenerTiempoGlobal() const;
    uint32_t obtenerPidEnEjecucion() const;
    const std::unordered_map<uint32_t, Proceso>& obtenerTablaProcesos() const;
    std::queue<uint32_t> obtenerColaListos() const;
    std::queue<uint32_t> obtenerColaSuspendidos() const;
    
    bool llamadaAlSistema(TipoLlamada tipo, std::string recurso, GestorComunicacion& ipc);
};

#endif // PLANIFICADOR_H