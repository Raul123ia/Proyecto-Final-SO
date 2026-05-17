// Refactorización de las funciones globales
#include "core/Planificador/Planificador.h"
#include "core/ProductorConsumidor/ProductorConsumidor.h"

void ProductorConsumidor::simularProductor(Planificador& Planificador, GestorComunicacion& ipc, GestorLogs& logs, uint32_t pid, int item) {
    // 1. Obtenemos el PCB actual (solo lectura, es lo seguro)
    const Proceso& p = Planificador.obtenerDetallesProceso(pid);
    int cp = p.obtenerContadorPrograma();

    // 2. Máquina de estados usando el PC
    switch(cp) {
        case 0:
            logs.anotarEvento("[Productor PID:" + std::to_string(pid) + "] Intentando producir item " + std::to_string(item) + "...");
            
            // SYSTEM CALL: WAIT (Espacios Vacíos)
            if (!Planificador.llamadaAlSistema(TipoLlamada::WAIT_SEMAFORO, "Espacios_Vacios", ipc)) {
                logs.anotarEvento("[Productor PID:" + std::to_string(pid) + "] SysCall Bloqueante: Buffer lleno.");
                return; // El proceso fue suspendido por el Kernel. Salimos.
            }
            // Éxito. Avanzamos el PC.
            Planificador.actualizarContadorPrograma(pid, 1);
            // No hay break, queremos que siga ejecutando (fallthrough)

        case 1:
            // SYSTEM CALL: WAIT (Mutex)
            if (!Planificador.llamadaAlSistema(TipoLlamada::WAIT_SEMAFORO, "Mutex", ipc)) {
                logs.anotarEvento("[Productor PID:" + std::to_string(pid) + "] SysCall Bloqueante: Mutex ocupado.");
                return; // Suspendido.
            }
            Planificador.actualizarContadorPrograma(pid, 2);

        case 2: {
            // --- SECCIÓN CRÍTICA (ATÓMICA ESTRICTA) ---
            std::lock_guard<std::mutex> lock(seccion_critica_mtx);
            
            // Verificación física final antes de modificar la memoria
            if (buffer_compartido.size() < CAPACIDAD_MAXIMA) {
                buffer_compartido.push(item);
                logs.anotarEvento("[Productor PID:" + std::to_string(pid) + "] Produjo el item " + std::to_string(item) + " exitosamente.");
            } else {
                logs.anotarEvento("[Productor PID:" + std::to_string(pid) + "] Produccion descartada para proteger desbordamiento físico.");
            }
            
            // SYSTEM CALLS: SIGNALS
            Planificador.llamadaAlSistema(TipoLlamada::SIGNAL_SEMAFORO, "Mutex", ipc);
            Planificador.llamadaAlSistema(TipoLlamada::SIGNAL_SEMAFORO, "Items_Disponibles", ipc);
            
            // Tarea finalizada. Reseteamos el PC por si este Productor vuelve a ser llamado.
            Planificador.actualizarContadorPrograma(pid, 0);
            break;
        }
    }
}

void ProductorConsumidor::simularConsumidor(Planificador& Planificador, GestorComunicacion& ipc, GestorLogs& logs, uint32_t pid) {
    const Proceso& p = Planificador.obtenerDetallesProceso(pid);
    int cp = p.obtenerContadorPrograma();

    switch(cp) {
        case 0:
            logs.anotarEvento("[Consumidor PID:" + std::to_string(pid) + "] Intentando consumir un item...");
            
            // SYSTEM CALL: WAIT (Items Disponibles)
            if (!Planificador.llamadaAlSistema(TipoLlamada::WAIT_SEMAFORO, "Items_Disponibles", ipc)) {
                logs.anotarEvento("[Consumidor PID:" + std::to_string(pid) + "] SysCall Bloqueante: Buffer vacio.");
                return; 
            }
            Planificador.actualizarContadorPrograma(pid, 1);

        case 1:
            // SYSTEM CALL: WAIT (Mutex)
            if (!Planificador.llamadaAlSistema(TipoLlamada::WAIT_SEMAFORO, "Mutex", ipc)) {
                logs.anotarEvento("[Consumidor PID:" + std::to_string(pid) + "] SysCall Bloqueante: Mutex ocupado.");
                return; 
            }
            Planificador.actualizarContadorPrograma(pid, 2);

        case 2: {
            // --- SECCIÓN CRÍTICA (ATÓMICA ESTRICTA) ---
            std::lock_guard<std::mutex> lock(seccion_critica_mtx);
            
            int item_consumido = -1;
            if (!buffer_compartido.empty()) {
                item_consumido = buffer_compartido.front();
                buffer_compartido.pop();
            }
            logs.anotarEvento("[Consumidor PID:" + std::to_string(pid) + "] Consumio el item " + std::to_string(item_consumido) + ".");
            
            
            // SYSTEM CALLS: SIGNALS
            Planificador.llamadaAlSistema(TipoLlamada::SIGNAL_SEMAFORO, "Mutex", ipc);
            Planificador.llamadaAlSistema(TipoLlamada::SIGNAL_SEMAFORO, "Espacios_Vacios", ipc);
            
            Planificador.actualizarContadorPrograma(pid, 0);
            break;
        }
    }
}
