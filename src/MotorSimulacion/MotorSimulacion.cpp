#include "core/MotorSimulacion/MotorSimulacion.h"

// Inclusiones de módulos homologados
#include "core/Planificador/Planificador.h"
#include "core/GestorComunicacion/GestorComunicacion.h"
#include "core/GestorRecursos/GestorRecursos.h" 
#include "core/GestorLogs/GestorLogs.h"
#include "core/CausaTerminacion/CausaTerminacion.h"

MotorSimulacion::MotorSimulacion() 
    : planificador(TipoAlgoritmo::FCFS), // Inicializamos el kernel con FCFS por defecto
    recursos(&registros),               // ¡CONEXIÓN CLAVE!: Pasamos la dirección de los logs al hardware
    siguiente_item(1)
{
    // Aquí el cuerpo del constructor queda limpio.
    // La "magia" de la conexión ya ocurrió arriba.
}

void MotorSimulacion::iniciar(TipoAlgoritmo algoritmo, int quantum) {
    // Configura el planificador inicial
    // planificador = Planificador(algoritmo, quantum);
}

void MotorSimulacion::iniciarSimulacionProductorConsumidor(int rafaga, int prioridad, int memoria) {
    comunicacion.inicializarSemaforo("Mutex", 1);
    comunicacion.inicializarSemaforo("Espacios_Vacios", static_cast<int>(simulacion.obtenerCapacidadMaxima()));
    comunicacion.inicializarSemaforo("Items_Disponibles", 0);

    crearProceso("Productor", rafaga, prioridad, memoria, TipoProceso::PRODUCTOR);
    crearProceso("Consumidor", rafaga, prioridad, memoria, TipoProceso::CONSUMIDOR);
}
 //Método terminado, solo falta implementar .anotarEvento en GestorLogs para que el log de creación de proceso quede registrado.
void MotorSimulacion::crearProceso(std::string nombre, int rafaga, int prioridad, int memoria, TipoProceso tipo) {
    
    // PASO 1: Validar si el hardware tiene RAM disponible
    if (!recursos.validarDisponibilidadMemoria(memoria)) {
        // No hay memoria. El SO rechaza la creación del proceso.
        std::string mensajeError = "Error: No hay memoria para '" + nombre + "'.";
        // registros.anotarEvento(mensajeError);
        return; 
    }

    // PASO 2: Si llegamos aquí, sí hay RAM. Le decimos al Planificador que construya el PCB.
    // Asumo que tu crearYAsignarProceso devuelve el PID asignado.
    // Aquí se delegaría la creación del proceso al planificador, que a su vez interactuaría con el gestor de recursos para asignar memoria, etc.
    uint32_t nuevoPid = planificador.crearYAsignarProceso(nombre, prioridad, rafaga, memoria, tipo);

    // PASO 3: Apartamos físicamente la memoria en el GestorRecursos
    recursos.asignarMemoria(nuevoPid, memoria);

    // El registro de éxito ya lo maneja el GestorRecursos internamente gracias a su puntero a logs.
}

void MotorSimulacion::ejecutarPasoSiguiente() {
    // =========================================================
    // EL PIPELINE ESTRICTO DE 4 FASES (UN TICK DE RELOJ)
    // =========================================================
    
    // FASE 1: DESPACHADOR (CONTEXT SWITCH)
    planificador.ejecutarDespachador();

    // FASE 2: GESTIÓN DE EVENTOS E IPC
    uint32_t pidActual = planificador.obtenerPidEnEjecucion();
    if (pidActual != 0) {
        const Proceso& procesoEnEjecucion = planificador.obtenerDetallesProceso(pidActual);
        switch (procesoEnEjecucion.obtenerTipoProceso()) {
            case TipoProceso::PRODUCTOR:
                simulacion.simularProductor(planificador, comunicacion, pidActual, siguiente_item++);
                break;
            case TipoProceso::CONSUMIDOR:
                simulacion.simularConsumidor(planificador, comunicacion, pidActual);
                break;
            case TipoProceso::NORMAL:
            default:
                break;
        }
    }

    // FASE 3: EJECUCIÓN EN CPU
    // La CPU descuenta la ráfaga del proceso actual
    planificador.ejecutarCPU();

    // FASE 4: CRONÓMETRO Y LOGS
    planificador.avanzarTiempo();
    // registros.anotarEvento("Tick " + std::to_string(planificador.obtenerTiempoGlobal()) + " completado.");
}

bool MotorSimulacion::invocarLlamadaSistema(TipoLlamada tipo, std::string recurso) {
    // El Motor actúa como mediador: toma la petición del programa externo 
    // y se la pasa al Planificador, inyectando físicamente el GestorComunicacion.
    
    bool resultado = planificador.llamadaAlSistema(tipo, recurso, comunicacion);
    
    // Opcional: Anotar en los logs si hubo un bloqueo
    if (!resultado && tipo == TipoLlamada::WAIT_SEMAFORO) {
        std::string mensaje = "[IPC] Proceso bloqueado esperando el semáforo: " + recurso;
        // registros.anotarEvento(mensaje);
    }
    
    return resultado;
}
//Método para obtener memoria usada, que simplemente delega la consulta al GestorRecursos, asegurando que siempre refleje el estado real del sistema.
uint32_t MotorSimulacion::obtenerMemoriaUsada() const {
    return recursos.obtenerMemoriaUsada();
}

size_t MotorSimulacion::obtenerOcupacionBuffer() const {
    return simulacion.obtenerTamanioBuffer();
}

size_t MotorSimulacion::obtenerCapacidadBuffer() const {
    return simulacion.obtenerCapacidadMaxima();
}
//Getter para obtener los logs, que simplemente devuelve una referencia de solo lectura al GestorLogs, asegurando que la UI pueda acceder a los datos reales sin riesgo de modificar el estado interno del sistema.
const GestorLogs& MotorSimulacion::obtenerLogs() const {
    return registros;
}

const GestorRecursos& MotorSimulacion::obtenerRecursos() const 
{
    return recursos;
}

const Planificador& MotorSimulacion::obtenerPlanificador() const 
{
    return planificador;
}