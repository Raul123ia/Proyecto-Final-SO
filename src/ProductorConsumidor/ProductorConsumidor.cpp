// Refactorización de las funciones globales
#include "core/Planificador/Planificador.h"
#include "core/ProductorConsumidor/ProductorConsumidor.h"
#include <iostream>

void ProductorConsumidor::simularProductor(Planificador& Planificador, GestorComunicacion& ipc, uint32_t pid, int item) {
    // 1. Obtenemos el PCB actual (solo lectura, es lo seguro)
    const Proceso& p = Planificador.obtenerDetallesProceso(pid);
    int cp = p.obtenerContadorPrograma();

    // 2. Máquina de estados usando el PC
    switch(cp) {
        case 0:
            std::cout << "\n[Productor PID:" << pid << "] Intentando producir item " << item << "...\n";
            
            // SYSTEM CALL: WAIT (Espacios Vacíos)
            if (!Planificador.llamadaAlSistema(TipoLlamada::WAIT_SEMAFORO, "Espacios_Vacios", ipc)) {
                std::cout << "[Productor PID:" << pid << "] SysCall Bloqueante: Buffer lleno.\n";
                return; // El proceso fue suspendido por el Kernel. Salimos.
            }
            // Éxito. Avanzamos el PC.
            Planificador.actualizarContadorPrograma(pid, 1);
            // No hay break, queremos que siga ejecutando (fallthrough)

        case 1:
            // SYSTEM CALL: WAIT (Mutex)
            if (!Planificador.llamadaAlSistema(TipoLlamada::WAIT_SEMAFORO, "Mutex", ipc)) {
                std::cout << "[Productor PID:" << pid << "] SysCall Bloqueante: Mutex ocupado.\n";
                return; // Suspendido.
            }
            Planificador.actualizarContadorPrograma(pid, 2);

        case 2:
            // --- SECCIÓN CRÍTICA ---
            buffer_compartido.push(item); // Descomenta esto cuando tengas tu cola
            std::cout << "[Productor PID:" << pid << "] Produjo el item " << item << " exitosamente.\n";
            
            // SYSTEM CALLS: SIGNALS
            Planificador.llamadaAlSistema(TipoLlamada::SIGNAL_SEMAFORO, "Mutex", ipc);
            Planificador.llamadaAlSistema(TipoLlamada::SIGNAL_SEMAFORO, "Items_Disponibles", ipc);
            
            // Tarea finalizada. Reseteamos el PC por si este Productor vuelve a ser llamado.
            Planificador.actualizarContadorPrograma(pid, 0);
            break;
    }
}

void ProductorConsumidor::simularConsumidor(Planificador& Planificador, GestorComunicacion& ipc, uint32_t pid) {
    const Proceso& p = Planificador.obtenerDetallesProceso(pid);
    int cp = p.obtenerContadorPrograma();

    switch(cp) {
        case 0:
            std::cout << "\n[Consumidor PID:" << pid << "] Intentando consumir un item...\n";
            
            // SYSTEM CALL: WAIT (Items Disponibles)
            if (!Planificador.llamadaAlSistema(TipoLlamada::WAIT_SEMAFORO, "Items_Disponibles", ipc)) {
                std::cout << "[Consumidor PID:" << pid << "] SysCall Bloqueante: Buffer vacio.\n";
                return; 
            }
            Planificador.actualizarContadorPrograma(pid, 1);

        case 1:
            // SYSTEM CALL: WAIT (Mutex)
            if (!Planificador.llamadaAlSistema(TipoLlamada::WAIT_SEMAFORO, "Mutex", ipc)) {
                std::cout << "[Consumidor PID:" << pid << "] SysCall Bloqueante: Mutex ocupado.\n";
                return; 
            }
            Planificador.actualizarContadorPrograma(pid, 2);

        case 2:
            // --- SECCIÓN CRÍTICA ---
            int item_consumido = buffer_compartido.front();
            buffer_compartido.pop();
            std::cout << "[Consumidor PID:" << pid << "] Consumio el item " << item_consumido << ".\n";
            
            
            // SYSTEM CALLS: SIGNALS
            Planificador.llamadaAlSistema(TipoLlamada::SIGNAL_SEMAFORO, "Mutex", ipc);
            Planificador.llamadaAlSistema(TipoLlamada::SIGNAL_SEMAFORO, "Espacios_Vacios", ipc);
            
            Planificador.actualizarContadorPrograma(pid, 0);
            break;
    }
}
