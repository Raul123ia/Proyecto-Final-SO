#include "core/ProductorConsumidor/ProductorConsumidor.h"
#include <iostream>

// Función para simular que un proceso actuando como productor intenta depositar un item en el buffer
void SimulacionIPC::simularProductor(GestorComunicacion& ipc, uint32_t pid, int item) {
    // Registramos que este productor va a intentar algo
    std::cout << "\n[Productor PID:" << pid << "] Intentando producir item " << item << "...\n";
    
    // Primer WAIT: Verificamos si aún hay huecos vacíos en el buffer
    if (ipc.esperarSemaforo("Espacios_Vacios", pid)) {
        // Segundo WAIT: Asegurar la entrada a la sección crítica exclusión mutua
        if (ipc.esperarSemaforo("Mutex", pid)) {
            
            // Hemos logrado entrar! Añadimos el item generado hacia el final de nuestra cola/buffer simulado
            buffer_compartido.push(item); 
            // Mensaje de éxito informando que se introdujo
            std::cout << "[Productor PID:" << pid << "] Produjo el item " << item << " exitosamente.\n";
            
            // IMPORTANTE: el orden aquí evita deadlocks. Liberamos Mutex con SIGNAL.
            ipc.liberarSemaforo("Mutex");
            
            // Finalmente damos SIGNAL al semáforo protector de número de items, avisando que existe 1 item más para consumir
            ipc.liberarSemaforo("Items_Disponibles");
            
        } else {
            // El proceso quedó bloqueado porque perdió Mutex a manos de otro ente después de comprobar que había vacío
            std::cout << "[Productor PID:" << pid << "] Bloqueado al momento de adquirir Mutex.\n";
        }
    } else {
        // El proceso quedó bloqueado por tratar de producir cuando el buffer está en su máxima capacidad y el contador dio 0
        std::cout << "[Productor PID:" << pid << "] Bloqueado: Buffer lleno.\n";
    }
}

// Función que simula al consumidor, este se encarga de retirar tareas del buffer compartido
void SimulacionIPC::simularConsumidor(GestorComunicacion& ipc, uint32_t pid) {
    // Registramos inicio de la acción consumidora
    std::cout << "\n[Consumidor PID:" << pid << "] Intentando consumir un item...\n";
    
    // Primer WAIT: Chequeamos que existan datos listos para sacar de la cola
    if (ipc.esperarSemaforo("Items_Disponibles", pid)) {
        // Segundo WAIT: Pedir la perilla universal Mutex para poder tocar la colección sin chocar
        if (ipc.esperarSemaforo("Mutex", pid)) {
            
            // Copiamos el dato situado enfrente de nuestra cola y sacamos una copia al exterior. 
            int item_consumido = buffer_compartido.front();
            buffer_compartido.pop(); // Destruimos el mismo del buffer compartido
            // Notificamos satisfactoriamente la obtención de datos
            std::cout << "[Consumidor PID:" << pid << "] Consumio el item " << item_consumido << ".\n";
            
            // Restituimos la variable exclusión mutua vía función SIGNAL para que un productor regrese
            ipc.liberarSemaforo("Mutex");
            
            // Se le suma 1 al semáforo de agujeros (SIGNAL) notificando que existe una posición de buffer libre más
            ipc.liberarSemaforo("Espacios_Vacios");
            
        } else {
             // El flujo detectó Mutex en 0 y se metió al queue sin obtener acceso al código seguro
            std::cout << "[Consumidor PID:" << pid << "] Bloqueado al momento de adquirir Mutex.\n";
        }
    } else {
        // El flujo frenó su paso dado que el contador indicó que hay un total de 0 productos actualmente 
        std::cout << "[Consumidor PID:" << pid << "] Bloqueado: Buffer vacio.\n";
    }
}
