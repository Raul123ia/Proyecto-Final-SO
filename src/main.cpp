#include <iostream>
#include <queue>
#include "GestorComunicacion.hpp"

// Buffer global compartido entre Productor y Consumidor
std::queue<int> buffer;
const int CAPACIDAD_MAXIMA = 3;

// ---------------------------------------------------------
// Productor
// ---------------------------------------------------------
void simularProductor(GestorComunicacion& ipc, Proceso* p, int item) {
    std::cout << "\n--> [" << p->nombre << "] Intenta producir el elemento: " << item << std::endl;
    
    // 1. Verificar si hay espacio libre en el buffer (WAIT a Espacios_Vacios)
    bool espacios_concedido = ipc.esperarSemaforo("Espacios_Vacios", p);
    if (!espacios_concedido) {
        std::cout << "    [BLOQUEADO] No hay espacio, el Productor se duerme." << std::endl;
        return; // El proceso se bloquearía en un SO real
    }
    
    // 2. Solicitar acceso exclusivo para modificar el buffer (WAIT a Mutex)
    bool mutex_concedido = ipc.esperarSemaforo("Mutex", p);
    if (!mutex_concedido) {
        return; // Se bloquearía si alguien más está dentro
    }
    
    // 3. SECCIÓN CRÍTICA: Insertar el elemento libre de peligro
    buffer.push(item);
    std::cout << "    [EXITO] Item " << item << " agregado al buffer." << std::endl;
    
    // 4. Liberar la exclusión mutua para que otros puedan entrar (SIGNAL a Mutex)
    ipc.liberarSemaforo("Mutex");
    
    // 5. Avisar a un posible consumidor que ya hay elementos disponibles (SIGNAL a Items_Disponibles)
    ipc.liberarSemaforo("Items_Disponibles");
}

// ---------------------------------------------------------
// Consumidor
// ---------------------------------------------------------
void simularConsumidor(GestorComunicacion& ipc, Proceso* p) {
    std::cout << "\n--> [" << p->nombre << "] Intenta consumir un elemento." << std::endl;
    
    // 1. Verificar si existen elementos producidos en el buffer (WAIT a Items_Disponibles)
    bool items_validos = ipc.esperarSemaforo("Items_Disponibles", p);
    if (!items_validos) {
        std::cout << "    [BLOQUEADO] Buffer vacio, el Consumidor se duerme." << std::endl;
        return; // El proceso se bloquearía en un SO real
    }
    
    // 2. Pedir acceso exclusivo al arreglo (WAIT a Mutex)
    bool mutex_concedido = ipc.esperarSemaforo("Mutex", p);
    if (!mutex_concedido) {
        return; // Se bloquearía si otro manipula el buffer
    }
    
    // 3. SECCIÓN CRÍTICA: Extraer el elemento
    int valor_extraido = buffer.front();
    buffer.pop();
    std::cout << "    [EXITO] Se extrajo y consumió el item: " << valor_extraido << std::endl;
    
    // 4. Soltar acceso exclusivo al buffer (SIGNAL a Mutex)
    ipc.liberarSemaforo("Mutex");
    
    // 5. Anunciar que ha quedado un hueco nuevo en el buffer (SIGNAL a Espacios_Vacios)
    ipc.liberarSemaforo("Espacios_Vacios");
}

int main() {
    std::cout << "--- INICIANDO GESTOR DE COMUNICACION Y SINCRO ---" << std::endl;
    
    GestorComunicacion ipc;
    
    // Inicialización de Semáforos para el Productor/Consumidor
    ipc.inicializarSemaforo("Mutex", 1); 
    ipc.inicializarSemaforo("Espacios_Vacios", CAPACIDAD_MAXIMA);
    ipc.inicializarSemaforo("Items_Disponibles", 0);
    
    Proceso* productor1 = new Proceso(100, "Productor1");
    Proceso* consumidor1 = new Proceso(200, "Consumidor1");
    
    // Casos de Escenario
    simularConsumidor(ipc, consumidor1); // Debería bloquearse (buffer vacio)
    
    simularProductor(ipc, productor1, 42); // Produce 42
    simularProductor(ipc, productor1, 77); // Produce 77
    
    simularConsumidor(ipc, consumidor1); // Debería consumir 42
    
    delete productor1;
    delete consumidor1;
    
    return 0;
}   