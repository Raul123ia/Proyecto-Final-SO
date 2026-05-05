#pragma once // Evita que este archivo se incluya más de una vez durante la compilación

#include <deque> // Permite usar la estructura de datos deque (cola doble)
#include <vector> // Permite usar la estructura de datos vector
#include <cstdint> // Define tipos de enteros de tamaño fijo como uint32_t
#include <string_view> // Permite usar vistas de cadenas sin copiar datos
#include "../core/GestorRecursos.h" // Incluye la clase GestorRecursos para manejar CPU y memoria

// Enumeración para las causas de terminación de un proceso
enum class CausaTerminacion {
    NORMAL, // El proceso terminó correctamente
    ERROR, // El proceso terminó por algún error
    INTERBLOQUEO, // El proceso terminó por deadlock (interbloqueo)
    USUARIO // El proceso fue terminado manualmente por el usuario
};

// Función auxiliar evaluada en tiempo de compilación para convertir el enum a texto
constexpr std::string_view causaToString(CausaTerminacion causa) { // Recibe una causa y devuelve su representación en texto
    switch (causa) { // Evalúa el valor del enum
        case CausaTerminacion::NORMAL: return "NORMAL"; // Si es NORMAL, devuelve "NORMAL"
        case CausaTerminacion::ERROR: return "ERROR"; // Si es ERROR, devuelve "ERROR"
        case CausaTerminacion::INTERBLOQUEO: return "INTERBLOQUEO"; // Si es INTERBLOQUEO, devuelve su texto
        case CausaTerminacion::USUARIO: return "USUARIO"; // Si es USUARIO, devuelve su texto
        default: return "DESCONOCIDA"; // En caso de valor no contemplado, devuelve "DESCONOCIDA"
    }
}

// nodiscard obliga a comprobar si la finalización fue exitosa
// Ahora usamos uint32_t (PIDs) en lugar de Proceso*
[[nodiscard]] bool finalizar(uint32_t pid, // PID del proceso a finalizar
                             CausaTerminacion causa, // Motivo de terminación
                             std::deque<uint32_t>& cola_listos, // Referencia a la cola de procesos listos
                             std::vector<uint32_t>& historial_terminados, // Lista de procesos ya terminados
                             uint32_t& pid_en_ejecucion, // PID del proceso en ejecución (0 si no hay ninguno)
                             GestorRecursos& recursos); // Referencia al gestor de recursos para liberar CPU y memoria