#include "core/MotorSimulacion/MotorSimulacion.h"

// Inclusiones de módulos homologados
#include "core/Planificador/Planificador.h"
#include "core/GestorComunicacion/GestorComunicacion.h"
#include "core/GestorRecursos/GestorRecursos.h" 
#include "core/GestorLogs/GestorLogs.h"
#include "core/CausaTerminacion/CausaTerminacion.h"
#include "core/ProductorConsumidor/ProductorConsumidor.h"
#include "core/Planificador/Proceso.h"
#include <random>
    
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
//  Método para intentar meter procesos a RAM
void MotorSimulacion::intentarCargarProcesosAMemoria() {
    size_t ocupacionBuffer = obtenerOcupacionBuffer();
    size_t capacidadBuffer = obtenerCapacidadBuffer();

    auto it = cola_nuevos.begin();
    while (it != cola_nuevos.end()) {
        const auto& p = *it;
        
        // Logica de admision inteligente (Prevencion de Deadlock a largo plazo)
        bool esProductor = (p.tipo == TipoProceso::PRODUCTOR);
        bool esConsumidor = (p.tipo == TipoProceso::CONSUMIDOR);
        
        // 1. Si Buffer Lleno: Dar prioridad a consumidores (ignorar productores)
        if (ocupacionBuffer == capacidadBuffer && esProductor) {
            ++it; // Saltamos este productor temporalmente
            continue;
        }
        
        // 2. Si Buffer Vacio: Dar prioridad a productores (ignorar consumidores temporalmente opcional o dejarlos pasar? El prompt dice "prioridad de entrada a los Productores". Lo mas seguro es no bloquear a nadie a menos de que en serio este vacio y solo queremos productores. Para no ser tan estrictos, simplemente permitimos todo normal, a menos que si queramos saltar consumidores cuando esta vacio. "El admisor debe darle prioridad de entrada a los Productores". Podriamos retrasar consumidores si esta vacio).
        if (ocupacionBuffer == 0 && esConsumidor) {
            ++it; // Saltamos consumidor porque no tiene nada que leer
            continue;
        }

        if (validarMemoriaProceso(p.memoria)) {
            // Si cabe, lo creamos de verdad y lo metemos al Planificador
            crearProceso(p.nombre, p.rafaga, p.prioridad, p.memoria, p.tipo);
            it = cola_nuevos.erase(it); // Lo sacamos de la sala de espera
            
            // Re-evaluar por si este u otros afectaron algo de los recursos
            // No podemos saber la ocupacion real del buffer si el proceso todavia no es agendado,
            // pero si evitamos admitir puros productores si el buffer esta lleno, esto detiene el deadlock.
        } else {
            // Si no hay RAM para el proceso actual que SÍ queríamos admitir,
            // continuamos iterando por si hay un proceso más pequeño en RAM o un Consumidor que sí quepa (First-Fit / Next-Fit dinámico en vez de FCFS estricto).
            ++it; 
        }
    }
}
bool MotorSimulacion::validarMemoriaProceso(uint32_t memoria) const {
    return recursos.validarDisponibilidadMemoria(memoria);
}

bool MotorSimulacion::asignarMemoriaProceso(uint32_t pid,uint32_t memoria) {
    return recursos.asignarMemoria(pid, memoria);
}

uint32_t MotorSimulacion::liberarMemoriaProceso(uint32_t pid) {
    return recursos.liberarMemoria(pid);
}
// ================= CREACIÓN DE PROCESOS DE FORMA AUTOMÁTICA =================
void MotorSimulacion::inicializarSimulacionAutomatica(int cantidad_procesos) {
    // 1. Inicializamos los semáforos de IPC de una vez
    comunicacion.inicializarSemaforo("Mutex", 1);
    comunicacion.inicializarSemaforo("Espacios_Vacios", 5);
    comunicacion.inicializarSemaforo("Items_Disponibles", 0);

    // 2. Herramientas para números aleatorios
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distRafaga(5, 20); // Ráfagas entre 5 y 20
    std::uniform_int_distribution<> distPrioridad(1, 5); 
    std::uniform_int_distribution<> distMemoria(64, 1024); // Memoria entre 64MB y 1GB
    std::uniform_int_distribution<> distTipo(1, 100); // Para probabilidad

    // 3. Generamos todos los procesos y los mandamos a la sala de espera
    for (int i = 1; i <= cantidad_procesos; ++i) {
        std::string nombre = "P" + std::to_string(i);
        TipoProceso tipo = TipoProceso::NORMAL;

        // 20% de probabilidad de ser Productor o Consumidor
        int prob = distTipo(gen);
        if (prob <= 10) tipo = TipoProceso::PRODUCTOR;
        else if (prob <= 20) tipo = TipoProceso::CONSUMIDOR;

        ProcesoPendiente p = {nombre, distRafaga(gen), distPrioridad(gen), distMemoria(gen), tipo};
        
        cola_nuevos.push_back(p); // Los dejamos en la cola de Nuevos esperando RAM
    }

    registros.anotarEvento("[Sistema] Simulación inicializada con " + std::to_string(cantidad_procesos) + " procesos generados.");
}
// ================= CPU =================
bool MotorSimulacion::asignarCPUProceso(uint32_t pid) {
    return recursos.asignarCPU(pid);
}

void MotorSimulacion::liberarCPUProceso(uint32_t pid) {
    recursos.liberarCPU(pid);
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

void MotorSimulacion::limpiarProcesosHuerfanos() {
    auto suspendidos = planificador.obtenerColaSuspendidos();
    if (suspendidos.empty()) return;

    // Tarea 4: Inyectamos tokens masivos para que ningún WAIT vuelva a bloquear a los procesos restantes
    // Así podrán pasar libremente por sus estados hasta consumir toda su ráfaga y morir limpiamente.
    comunicacion.inicializarSemaforo("Espacios_Vacios", 99999);
    comunicacion.inicializarSemaforo("Items_Disponibles", 99999);
    comunicacion.inicializarSemaforo("Mutex", 99999);

    // Los pasamos todos a la cola de listos
    for (uint32_t pid : suspendidos) {
        planificador.reanudar(pid);
    }
    
    registros.anotarEvento("[Sistema] Inactividad detectada. Iniciando protocolo Flush: Liberando " + std::to_string(suspendidos.size()) + " procesos huérfanos.");
}

void MotorSimulacion::establecerAlgoritmoPlanificacion(TipoAlgoritmo algoritmo) {
    planificador.establecerAlgoritmo(algoritmo);
}

TipoAlgoritmo MotorSimulacion::obtenerAlgoritmoPlanificacion() const {
    return planificador.obtenerAlgoritmo();
}

Planificador& MotorSimulacion::obtenerPlanificador() {
    return planificador;
}

void MotorSimulacion::ejecutarPasoSiguiente() {
    // =========================================================
    // EL PIPELINE ESTRICTO DE 4 FASES (UN TICK DE RELOJ)
    // =========================================================
    // Antes de cualquier cosa, intentamos cargar procesos nuevos a RAM (si es que hay)
    intentarCargarProcesosAMemoria();
    // FASE 1: DESPACHADOR (CONTEXT SWITCH)
    planificador.ejecutarDespachador();

    // FASE 2: GESTIÓN DE EVENTOS E IPC
    uint32_t pidActual = planificador.obtenerPidEnEjecucion();
    if (pidActual != 0) {
        const Proceso& procesoEnEjecucion = planificador.obtenerDetallesProceso(pidActual);
        switch (procesoEnEjecucion.obtenerTipoProceso()) {
            case TipoProceso::PRODUCTOR:
                simulacion.simularProductor(planificador, comunicacion, registros, pidActual, siguiente_item++);
                break;
            case TipoProceso::CONSUMIDOR:
                simulacion.simularConsumidor(planificador, comunicacion, registros, pidActual);
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

    // Tarea 3: Rutina de rescate final
    // Si ya no quedan procesos nuevos por llegar, la cola de listos está vacía
    // y la CPU está libre, significa que la simulación se estancó con puros huérfanos.
    if (cola_nuevos.empty() && 
        planificador.obtenerColaListos().empty() && 
        planificador.obtenerPidEnEjecucion() == 0) {
        
        limpiarProcesosHuerfanos();
    }
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

size_t MotorSimulacion::obtenerProcesosNuevosPendientes() const {
    return cola_nuevos.size();
}