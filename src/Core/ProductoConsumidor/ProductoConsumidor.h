#ifndef SIMULACION_IPC_HPP
#define SIMULACION_IPC_HPP

#include <queue>
#include <cstdint>
#include "../GestorComunicacion/GestorComunicacion.h"

class SimulacionIPC {
private:
    std::queue<int> buffer_compartido;

public:
    void simularProductor(GestorComunicacion& ipc, uint32_t pid, int item);
    void simularConsumidor(GestorComunicacion& ipc, uint32_t pid);
};

#endif // SIMULACION_IPC_HPP
