#include "./Planificador.h"

// Constructor
Planificador::Planificador(TipoAlgoritmo _algoritmo, int _quantum)
    : algoritmo(_algoritmo), valor_quantum(_quantum), 
    reloj_global(0), 
    pid_en_ejecucion(0),
     ticks_ejecutados_quantum(0) {
}

// API de solo lectura para la UI
const Proceso& Planificador::obtenerDetallesProceso(uint32_t pid) const {
    // .at() lanza una excepción std::out_of_range si el PID no existe,
    // protegiendo la integridad de la memoria.
    return tabla_procesos.at(pid);
}

// Creación y registro de procesos
uint32_t Planificador::crearYAsignarProceso(std::string nombre, int prioridad, uint64_t rafaga, uint32_t memoria) {
    // 1. Instanciamos el proceso
    Proceso nuevo_proceso(nombre, prioridad, rafaga, memoria);
    uint32_t nuevo_pid = nuevo_proceso.obtenerPid();
    
    // 2. Lo guardamos en el mapa. Usamos emplace porque es más eficiente que insert.
    tabla_procesos.emplace(nuevo_pid, nuevo_proceso);
    
    // 3. Formamos su PID en la cola de listos
    cola_listos.push(nuevo_pid);
    
    return nuevo_pid;
}

/* Este método es el motor de la simulación. 
Cada vez que se llama, avanza el tiempo y ejecuta la lógica del planificador.*/
void Planificador::ejecutarDespachador() {
    // Si la CPU está ocupada, la lógica depende del algoritmo (Apropiativo vs No Apropiativo)
    switch (algoritmo) {
        case TipoAlgoritmo::FCFS:
            // No apropiativo: Si la CPU está libre, tomamos al primero de la fila
            if (pid_en_ejecucion == 0 && !cola_listos.empty()) {
                pid_en_ejecucion = cola_listos.front();
                cola_listos.pop();
                tabla_procesos.at(pid_en_ejecucion).actualizarEstado(EstadoProceso::EJECUTANDO);
            }
            break;

        case TipoAlgoritmo::ROUND_ROBIN:
            // Aquí irá tu lógica de RR: 
            // Si el proceso actual agotó su quantum, lo sacas (preemption),
            // lo mandas al final de la cola, y metes al siguiente.
            break;

        case TipoAlgoritmo::SJF:
            // Aquí irá tu lógica de SJF:
            // Si la CPU está libre, en lugar de tomar el .front(), 
            // tendrás que buscar en la cola cuál tiene la rafaga_restante más corta.
            break;

        case TipoAlgoritmo::PRIORIDADES:
            // Lógica de prioridades...
            break;
    }
}

// 1. Solo avanza el tiempo matemáticamente (Ideal para tu botón de la UI)
void Planificador::avanzarTiempo() {
    reloj_global++;
}


// 3. Simula el trabajo físico del procesador (Hardware)
void Planificador::ejecutarCPU() {
    if (pid_en_ejecucion != 0) {
        Proceso& proceso_actual = tabla_procesos.at(pid_en_ejecucion);
        
        // AQUÍ es donde se resta el tiempo de ráfaga (tu punto 2)
        proceso_actual.ejecutarUnTick(); 

        // Verificamos si terminó
        if (proceso_actual.obtenerRafagaRestante() == 0) {
            proceso_actual.actualizarEstado(EstadoProceso::TERMINADO);
            historial_terminados.push_back(pid_en_ejecucion);
            pid_en_ejecucion = 0; 
        }
    }
}

// Opcional: Un método aglomerador por si quieren correr todo el ciclo de un golpe
void Planificador::ejecutarCicloCompleto() {
    avanzarTiempo();
    ejecutarDespachador();
    ejecutarCPU();
}


// Getters de estado global
uint64_t Planificador::obtenerTiempoGlobal() const { return reloj_global; }
uint32_t Planificador::obtenerPidEnEjecucion() const { return pid_en_ejecucion; }

// Stubs (métodos vacíos por ahora para que el código compile sin errores).
// Se llenarán cuando el equipo conecte el gestor de recursos o el IPC.
void Planificador::suspender(uint32_t pid) {
    // Futura lógica para mover un proceso a cola_suspendidos
}

void Planificador::reanudar(uint32_t pid) {
    // Futura lógica para regresarlo a cola_listos
}

void Planificador::finalizar(uint32_t pid, CausaTerminacion causa) {
    // Futura lógica para matar un proceso prematuramente
}