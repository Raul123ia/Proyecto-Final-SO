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
// Implementación del manejador de Llamadas al Sistema
bool Planificador::llamadaAlSistema(TipoLlamada tipo, std::string recurso, GestorComunicacion& ipc) {
    
    // 1. Validación de seguridad (Protección de Memoria/Modo Kernel)
    // Solo un proceso que actualmente tiene el control de la CPU puede hacer una System Call.
    if (pid_en_ejecucion == 0) {
        return false; // Error crítico: Llamada fantasma
    }

    // Identificamos quién está haciendo la petición
    uint32_t pid_solicitante = pid_en_ejecucion;

    // 2. Evaluamos qué tipo de servicio de Kernel solicitó
    switch (tipo) {
        
        case TipoLlamada::WAIT_SEMAFORO: {
            // Delegamos la petición de permisos al módulo IPC de tu compañero
            bool recurso_obtenido = ipc.esperarSemaforo(recurso, pid_solicitante);
            
            if (!recurso_obtenido) {
                // [Lógica del OS]: El IPC denegó el acceso (retornó false).
                // El proceso debe abandonar la CPU inmediatamente.
                
                // suspender(pid_solicitante); // TODO: Implementar más al rato
                
                // NOTA: Al suspenderlo, la variable pid_en_ejecucion deberá volver a 0 
                // para que el Despachador meta a otro proceso en el siguiente ciclo.
            }
            
            return recurso_obtenido; // Retornamos true si pasó, false si se bloqueó
        }

        case TipoLlamada::SIGNAL_SEMAFORO: {
            // El proceso avisa que ya terminó de usar el recurso
            uint32_t pid_despertado = ipc.liberarSemaforo(recurso);
            
            if (pid_despertado != 0) {
                // [Lógica del OS]: El IPC nos avisa que alguien estaba esperando este recurso 
                // y acaba de ganar el acceso. Hay que sacarlo de su estado de bloqueo.
                
                // reanudar(pid_despertado); // TODO: Implementar más al rato
                
                // NOTA: Al reanudarlo, este proceso pasará a la cola_listos para 
                // volver a competir por la CPU en el futuro.
            }
            
            return true; // Un SIGNAL siempre es una operación exitosa para quien lo llama
        }

        default:
            // Trampa para atrapar llamadas no implementadas
            return false; 
    }
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
            // Si el proceso actual agotó su quantum, lo sacas (preemption),
            // lo mandas al final de la cola, y metes al siguiente.
            // 1. ¿Hay alguien en la CPU y ya se le acabó el tiempo (Quantum)?
            if (pid_en_ejecucion != 0 && ticks_ejecutados_quantum >= valor_quantum) {
                // ¡EXPROPIACIÓN! (Context Switch forzado)
                Proceso& p_actual = tabla_procesos.at(pid_en_ejecucion);
                
                // Lo bajamos de la CPU y lo ponemos en estado LISTO
                p_actual.actualizarEstado(EstadoProceso::LISTO);
                
                // Lo mandamos al final de la fila de listos (si es que no terminó por su cuenta)
                cola_listos.push(pid_en_ejecucion); 
                
                // Liberamos la CPU y reseteamos el cronómetro
                pid_en_ejecucion = 0;
                ticks_ejecutados_quantum = 0;
            }

            // 2. Si la CPU está libre (ya sea porque el proceso terminó por su cuenta,
            // o porque lo acabamos de expropiar en el paso anterior)
            if (pid_en_ejecucion == 0 && !cola_listos.empty()) {
                pid_en_ejecucion = cola_listos.front();
                cola_listos.pop();
                
                tabla_procesos.at(pid_en_ejecucion).actualizarEstado(EstadoProceso::EJECUTANDO);
                ticks_ejecutados_quantum = 0; // Iniciamos el cronómetro para el nuevo proceso
            }
            break;

        case TipoAlgoritmo::SJF:
            // Si la CPU está libre, en lugar de tomar el .front(), 
            // tendrás que buscar en la cola cuál tiene la rafaga_restante más corta.
            break;

        case TipoAlgoritmo::PRIORIDADES:
            // Se elige al proceso con mayor prioridad (número más bajo) de la cola de listos.
            // Si hay empate en prioridad, se puede usar FCFS como criterio de desempate.
            // Para esto, probablemente quieras cambiar la cola_listos a una estructura de datos que te permita ordenar por prioridad (como un std::priority_queue) o hacer una búsqueda lineal cada vez que necesites despachar.
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
        
        // AQUÍ es donde se resta el tiempo de ráfaga 
        proceso_actual.ejecutarUnTick(); 

        /*El hardware avisa que se consumió un tick del quantum
         (Solo relevante para Round Robin)
        */
        ticks_ejecutados_quantum++; 

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