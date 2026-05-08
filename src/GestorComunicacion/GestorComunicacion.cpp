#include "../Core/GestorComunicacion/GestorComunicacion.h"

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
        return true;  // Retornamos true, indicando que el proceso puede continuar
        
    } else {
        // Como el valor del semáforo es 0, no hay recursos disponibles en este momento
        procesos_bloqueados[nombre_recurso].push(pid); // Empujamos el PID a la cola de bloqueados asociada a este recurso
        return false; // Retornamos false, el planificador debe cambiar el estado a BLOCK o WAITING
    }
}

// ---------------------------------------------------------
// Operación SIGNAL / V del semáforo
// ---------------------------------------------------------
// Retorna el PID de un proceso despertado, o 0 si no había nadie
uint32_t GestorComunicacion::liberarSemaforo(std::string nombre_recurso) {
    semaforos[nombre_recurso]++; // Aumentamos 1 al valor del semáforo devolviendo el permiso para este recurso

    // Verificamos en el mapa si la cola de este recurso tiene a otros procesos esperando
    if (!procesos_bloqueados[nombre_recurso].empty()) {
        
        // Sacamos el PID del primer proceso que llegó a hacer fila
        uint32_t pid_despertado = procesos_bloqueados[nombre_recurso].front();
        procesos_bloqueados[nombre_recurso].pop(); // Lo removemos definitivamente de la cola
        
        // Se le asigna directamente el recurso al que despertamos (le restamos el permiso que acabamos de meter)
        semaforos[nombre_recurso]--; 
        
        // Retornamos su PID. Así quien lo llame puede mover a 'pid_despertado' devuelta a la cola Ready.
        return pid_despertado;
    }
    
    // Si la cola estaba vacía y no despertamos a ningún bloqueado, se retorna 0 indicando que el PID 0 es reservado / nadie
    return 0;
}