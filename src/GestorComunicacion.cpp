
#include "GestorComunicacion.hpp"

// Método extra para inicializar los recursos compartidos en la simulación
void GestorComunicacion::inicializarSemaforo(std::string nombre_recurso, int permisos_iniciales) {
    semaforos[nombre_recurso] = permisos_iniciales;
}

// ---------------------------------------------------------
// FASE 3: LA COLISIÓN (Operación WAIT / P del semáforo)
// ---------------------------------------------------------
// Este método lo llama el Planificador cuando el proceso en la CPU quiere un recurso
bool GestorComunicacion::esperarSemaforo(std::string nombre_recurso, Proceso* p) {
    
    // Evaluamos la lógica matemática del semáforo
    if (semaforos[nombre_recurso] > 0) {
        // El semáforo está en VERDE. El proceso lo toma y resta 1.
        semaforos[nombre_recurso]--;
        std::cout << "[IPC] Permiso concedido. El proceso " << p->nombre 
                  << " tomó el recurso: " << nombre_recurso << ".\n";
        
        return true; // Le avisamos al Planificador que el proceso puede seguir (RUNNING)
        
    } else {
        // El semáforo está en ROJO. El proceso choca y debe suspenderse.
        procesos_bloqueados[nombre_recurso].push(p); // Lo metemos a la cola de espera
        
        std::cout << "[IPC] Recurso ocupado! Proceso " << p->nombre 
                  << " suspendido en: " << nombre_recurso << ".\n";
        
        // Retorna false. Aquí tu compañero del Planificador debe capturar este 'false',
        // cambiar el estado del proceso a WAITING y dejar la CPU libre (nullptr).
        return false; 
    }
}

// ---------------------------------------------------------
// FASE 4: LA REANUDACIÓN (Operación SIGNAL / V del semáforo)
// ---------------------------------------------------------
// Se llama cuando un proceso termina de usar el recurso y lo suelta
Proceso* GestorComunicacion::liberarSemaforo(std::string nombre_recurso) {
    
    // El proceso suelta el recurso, sumamos 1.
    semaforos[nombre_recurso]++; 
    std::cout << "[IPC] Recurso " << nombre_recurso << " ha sido liberado.\n";

    // Revisamos quién estaba esperando este semáforo (El Despertar)
    if (!procesos_bloqueados[nombre_recurso].empty()) {
        
        // Sacamos al primer proceso que estaba haciendo fila
        Proceso* proceso_despertado = procesos_bloqueados[nombre_recurso].front();
        procesos_bloqueados[nombre_recurso].pop();
        
        // Como ya lo despertamos, se le asigna el recurso inmediatamente (restamos 1)
        semaforos[nombre_recurso]--; 

        std::cout << "[IPC] Despertando al proceso " << proceso_despertado->nombre 
                  << " para que vuelva a competir por la CPU.\n";
        
        // Retornamos el puntero del proceso. 
        return proceso_despertado;
    }
    
    // Si no había nadie esperando en la cola, retornamos nulo
    return nullptr;
}