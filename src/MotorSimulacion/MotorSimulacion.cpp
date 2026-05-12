#include "core/MotorSimulacion/MotorSimulacion.h"

// Inclusiones de módulos homologados

#include "core/GestorComunicacion/GestorComunicacion.h"
#include "core/GestorRecursos/GestorRecursos.h" 
#include "core/GestorLogs/GestorLogs.h"
#include "core/CausaTerminacion/CausaTerminacion.h"
#include "core/ProductorConsumidor/ProductorConsumidor.h"

MotorSimulacion::MotorSimulacion()
    : planificador(TipoAlgoritmo::FCFS), // Inicializamos el kernel con FCFS por defecto
      recursos(&registros)               // ¡CONEXIÓN CLAVE!: Pasamos la dirección de los logs al hardware
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
void MotorSimulacion::simularProductor(uint32_t pid, int item) {
    simulacionIPC.simularProductor(comunicacion, pid, item);
}

void MotorSimulacion::simularConsumidor(uint32_t pid) {
    simulacionIPC.simularConsumidor(comunicacion, pid);
}
 //Método terminado, solo falta implementar .anotarEvento en GestorLogs para que el log de creación de proceso quede registrado.
// ================= CREACIÓN DE PROCESO MEMORIA=================
bool MotorSimulacion::crearProceso(std::string nombre,int rafaga,int prioridad,int memoria) {

    uint32_t memReq = static_cast<uint32_t>(memoria);

    // Validar primero sin disparar el log duplicado de GestorRecursos
    const uint32_t memoriaDisponible = recursos.obtenerMemoriaMaxima() - recursos.obtenerMemoriaUsada();
    if (memReq > memoriaDisponible) {
        registros.logInsuficienteMemoria(memReq, memoriaDisponible);
        return false;
    }

    // 1. Crear el proceso en el planificador
    uint32_t pid = planificador.crearYAsignarProceso(nombre,prioridad,rafaga,memReq);

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
    
    uint32_t pidActual = planificador.obtenerPidEnEjecucion();

    // FASE 1: GESTIÓN DE EVENTOS E IPC
    if (pidActual != 0) {
        const Proceso& procesoEnEjecucion = planificador.obtenerDetallesProceso(pidActual);
        // Aquí podrías revisar si el proceso actual hizo una llamada al sistema que lo bloquea, etc.
        // Por ejemplo, si el proceso hizo una llamada a un semáforo y no se pudo satisfacer, lo bloqueamos:
        // if (planificador.llamadaAlSistema(TipoLlamada::WAIT_SEMAFORO, "sem1", comunicacion)) {
        //     registros.anotarEvento("[Kernel] Proceso " + std::to_string(pidActual) + " bloqueado esperando semáforo 'sem1'.");
        // }
    }

    // FASE 2: DESPACHADOR (CONTEXT SWITCH)
    planificador.ejecutarDespachador();

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
