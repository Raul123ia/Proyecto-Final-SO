#include "Finalizar.h" // Incluye la declaración de la función finalizar y dependencias relacionadas
#include <iostream> // Permite usar entrada y salida estándar (cout)
#include <algorithm> // Necesario para usar std::find

// Función que finaliza un proceso dado su PID
bool finalizar(uint32_t pid, // Identificador del proceso a finalizar
               CausaTerminacion causa, // Motivo por el cual el proceso termina
               std::deque<uint32_t>& cola_listos, // Referencia a la cola de procesos listos
               std::vector<uint32_t>& historial_terminados, // Lista donde se guardan los procesos ya finalizados
               uint32_t& pid_en_ejecucion, // PID del proceso actualmente en CPU
               GestorRecursos& recursos) { // Referencia al gestor de recursos del sistema

    bool encontrado = false; // Variable para verificar si el proceso fue encontrado

    // 1. Verificar si el proceso a finalizar es el que está ocupando la CPU actualmente
    if (pid_en_ejecucion == pid) { // Compara si el PID en ejecución coincide con el PID a finalizar
        pid_en_ejecucion = 0; // Libera la CPU asignando 0 (sin proceso en ejecución)
        encontrado = true; // Marca que el proceso fue encontrado
    }
    else { // Si no está en ejecución, buscarlo en la cola de listos

        // 2. Buscar el PID dentro de la cola de listos
        auto it = std::find(cola_listos.begin(), cola_listos.end(), pid); // Busca el PID en la cola

        if (it != cola_listos.end()) { // Si el iterador no llegó al final, el PID existe
            cola_listos.erase(it); // Elimina el proceso de la cola de listos
            encontrado = true; // Marca que el proceso fue encontrado
        }
    }

    // 3. Verificación de seguridad: Si no estaba ni en ejecución ni en la cola
    if (!encontrado) { // Si no se encontró el proceso en ningún lado
        return false; // Retorna falso indicando que no se pudo finalizar
    }

    // 4. Liberar los recursos de CPU y Memoria simultáneamente
    // Usamos el método que ya tienes programado en GestorRecursos
    recursos.terminarProceso(pid); // Libera todos los recursos asociados a ese PID

    // 5. Almacenar el PID en el historial
    historial_terminados.push_back(pid); // Agrega el PID a la lista de procesos finalizados

    // 6. Generar log de estado en consola
    std::cout << "Proceso " << pid << " terminado. Causa: " // Imprime mensaje inicial
              << causaToString(causa) << "\n"; // Imprime la causa de terminación convertida a texto

    return true; // Indica que el proceso fue finalizado correctamente
}