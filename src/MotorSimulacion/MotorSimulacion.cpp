#include "core/MotorSimulacion/MotorSimulacion.h"

// Inclusiones de módulos homologados
#include "core/Planificador/Planificador.h"
#include "core/GestorComunicacion/GestorComunicacion.h"
#include "core/GestorRecursos/GestorRecursos.h" 
#include "core/GestorLogs/GestorLogs.h"
#include "core/CausaTerminacion/CausaTerminacion.h"
#include "core/ProductorConsumidor/ProductorConsumidor.h"

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
// ================= RAM =================
bool MotorSimulacion::validarMemoriaProceso(uint32_t memoria) const {
    return recursos.validarDisponibilidadMemoria(memoria);
}

bool MotorSimulacion::asignarMemoriaProceso(uint32_t pid,uint32_t memoria) {
    return recursos.asignarMemoria(pid, memoria);
}

uint32_t MotorSimulacion::liberarMemoriaProceso(uint32_t pid) {
    return recursos.liberarMemoria(pid);
}
// ================= CPU =================
bool MotorSimulacion::asignarCPUProceso(uint32_t pid) {
    return recursos.asignarCPU(pid);
}

void MotorSimulacion::liberarCPUProceso(uint32_t pid) {
    recursos.liberarCPU(pid);
}

void MotorSimulacion::iniciarSimulacionProductorConsumidor(int rafaga, int prioridad, int memoria) {
    comunicacion.inicializarSemaforo("Mutex", 1);
    comunicacion.inicializarSemaforo("Espacios_Vacios", static_cast<int>(simulacion.obtenerCapacidadMaxima()));
    comunicacion.inicializarSemaforo("Items_Disponibles", 0);

    crearProceso("Productor", rafaga, prioridad, memoria, TipoProceso::PRODUCTOR);
    crearProceso("Consumidor", rafaga, prioridad, memoria, TipoProceso::CONSUMIDOR);
}
 //Método terminado, solo falta implementar .anotarEvento en GestorLogs para que el log de creación de proceso quede registrado.
// ================= CREACIÓN DE PROCESO MEMORIA=================
bool MotorSimulacion::crearProceso(std::string nombre, int rafaga, int prioridad, int memoria, TipoProceso tipo) {

    uint32_t memReq = static_cast<uint32_t>(memoria);

    // Validar primero sin disparar el log duplicado de GestorRecursos
    const uint32_t memoriaDisponible = recursos.obtenerMemoriaMaxima() - recursos.obtenerMemoriaUsada();
    if (memReq > memoriaDisponible) {
        registros.logInsuficienteMemoria(memReq, memoriaDisponible);
        return false;
    }

    // 1. Crear el proceso en el planificador
    uint32_t pid = planificador.crearYAsignarProceso(nombre, prioridad, rafaga, memReq, tipo);

    // 2. Asignar memoria al proceso (la validación ocurre dentro de asignarMemoria)
    if (!asignarMemoriaProceso(pid, memReq)) {
        const uint32_t disponibleActual = recursos.obtenerMemoriaMaxima() - recursos.obtenerMemoriaUsada();
        registros.logInsuficienteMemoria(memReq, disponibleActual);
        return false;
    }

    return true;

}
// ================= LIBERAR MEMORIA =================
void MotorSimulacion::liberarMemoriaRAM(uint32_t pid) {
    liberarMemoriaProceso(pid);
    planificador.actualizarMemoriaProceso(pid, 0);
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

    // Guardamos quién quedó realmente en CPU después del despacho,
    // porque ese es el proceso que puede terminar en esta iteración.
    const uint32_t pidEnCPU = planificador.obtenerPidEnEjecucion();

    // FASE 3: EJECUCIÓN EN CPU
    // La CPU descuenta la ráfaga del proceso actual
    planificador.ejecutarCPU();

    // Si el proceso que entró a CPU terminó en este tick, liberamos la memoria
    // desde la MISMA instancia de GestorRecursos que la asignó.
    if (pidEnCPU != 0 && planificador.obtenerPidEnEjecucion() == 0) {
        const Proceso& procesoTerminado = planificador.obtenerDetallesProceso(pidEnCPU);
        if (procesoTerminado.obtenerRafagaRestante() == 0) {
            liberarMemoriaRAM(pidEnCPU);
        }
    }

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
        registros.anotarEvento(mensaje);
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
