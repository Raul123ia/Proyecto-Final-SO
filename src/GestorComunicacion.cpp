
#include "GestorComunicacion.hpp"

// Variable global para simular el buffer del productor-consumidor (capacidad será controlada lógicamente por semáforos)
std::queue<int> buffer_global; // Cola para almacenar los items compartidos entre procesos

// Inicializa el semáforo con un nombre de recurso y un valor inicial de accesos
void GestorComunicacion::inicializarSemaforo(std::string nombre_recurso, int permisos_iniciales) {
    semaforos[nombre_recurso] = permisos_iniciales; // Guardamos el valor inicial del semáforo en el mapa
}

// ---------------------------------------------------------
// Operación WAIT / P del semáforo
// ---------------------------------------------------------
// Retorna true si obtuvo el recurso, o false si se tuvo que bloquear al encontrar valor 0
bool GestorComunicacion::esperarSemaforo(std::string nombre_recurso, uint32_t pid) {
    // Verificamos si la cuenta del semáforo es mayor a 0 (hay permisos disponibles)
    if (semaforos[nombre_recurso] > 0) {
        semaforos[nombre_recurso]--;  // Restamos 1 al semáforo porque el turno ha sido tomado por este proceso
        // Imprimimos el log indicando que el proceso consumió efectivamente un permiso
        std::cout << "[IPC] Permiso concedido. El proceso PID:" << pid 
                  << " tomo el recurso: " << nombre_recurso << ".\n";
        return true;  // Retornamos true, indicando que el proceso puede continuar
        
    } else {
        // Como el valor del semáforo es 0, no hay recursos disponibles en este momento
        procesos_bloqueados[nombre_recurso].push(pid); // Empujamos el PID a la cola de bloqueados asociada a este recurso
        
        // Imprimimos en consola que el proceso no logró obtener el recurso y debe bloquearse
        std::cout << "[IPC] Recurso ocupado! Proceso PID:" << pid 
                  << " suspendido en espera de: " << nombre_recurso << ".\n";
        
        return false; // Retornamos false, el planificador debe cambiar el estado a BLOCK o WAITING
    }
}

// ---------------------------------------------------------
// Operación SIGNAL / V del semáforo
// ---------------------------------------------------------
// Retorna el PID de un proceso despertado, o 0 si no había nadie
uint32_t GestorComunicacion::liberarSemaforo(std::string nombre_recurso) {
    semaforos[nombre_recurso]++; // Aumentamos 1 al valor del semáforo devolviendo el permiso para este recurso
    // Anunciamos por consola que se sumó 1 al semáforo (liberado)
    std::cout << "[IPC] Recurso " << nombre_recurso << " ha sido liberado.\n";

    // Verificamos en el mapa si la cola de este recurso tiene a otros procesos esperando
    if (!procesos_bloqueados[nombre_recurso].empty()) {
        
        // Sacamos el PID del primer proceso que llegó a hacer fila
        uint32_t pid_despertado = procesos_bloqueados[nombre_recurso].front();
        procesos_bloqueados[nombre_recurso].pop(); // Lo removemos definitivamente de la cola
        
        // Se le asigna directamente el recurso al que despertamos (le restamos el permiso que acabamos de meter)
        semaforos[nombre_recurso]--; 

        // Log que avisa al usuario quién fue la entidad que despertó y obtuvo el semáforo al instante
        std::cout << "[IPC] Despertando al proceso PID:" << pid_despertado 
                  << " para que vuelva a competir por la CPU.\n";
        
        // Retornamos su PID. Así quien lo llame puede mover a 'pid_despertado' devuelta a la cola Ready.
        return pid_despertado;
    }
    
    // Si la cola estaba vacía y no despertamos a ningún bloqueado, se retorna 0 indicando que el PID 0 es reservado / nadie
    return 0;
}

// ---------------------------------------------------------
// PROBLEMA DEL PRODUCTOR - CONSUMIDOR
// ---------------------------------------------------------

// Función para simular que un proceso actuando como productor intenta depositar un item en el buffer
void simularProductor(GestorComunicacion& ipc, uint32_t pid, int item) {
    // Registramos que este productor va a intentar algo
    std::cout << "\n[Productor PID:" << pid << "] Intentando producir item " << item << "...\n";
    
    // Primer WAIT: Verificamos si aún hay huecos vacíos en el buffer
    if (ipc.esperarSemaforo("Espacios_Vacios", pid)) {
        // Segundo WAIT: Asegurar la entrada a la sección crítica exclusión mutua
        if (ipc.esperarSemaforo("Mutex", pid)) {
            
            // Hemos logrado entrar! Añadimos el item generado hacia el final de nuestra cola/buffer simulado
            buffer_global.push(item); 
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
void simularConsumidor(GestorComunicacion& ipc, uint32_t pid) {
    // Registramos inicio de la acción consumidora
    std::cout << "\n[Consumidor PID:" << pid << "] Intentando consumir un item...\n";
    
    // Primer WAIT: Chequeamos que existan datos listos para sacar de la cola
    if (ipc.esperarSemaforo("Items_Disponibles", pid)) {
        // Segundo WAIT: Pedir la perilla universal Mutex para poder tocar la colección sin chocar
        if (ipc.esperarSemaforo("Mutex", pid)) {
            
            // Copiamos el dato situado enfrente de nuestra cola y sacamos una copia al exterior. 
            int item_consumido = buffer_global.front();
            buffer_global.pop(); // Destruimos el mismo del buffer compartido
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