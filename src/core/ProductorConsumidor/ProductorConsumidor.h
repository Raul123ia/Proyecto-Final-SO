#ifndef PRODUCTOR_CONSUMIDOR_HPP
#define PRODUCTOR_CONSUMIDOR_HPP

#include <queue>
#include <cstdint>
#include "../GestorComunicacion/GestorComunicacion.h"
#include "../Planificador/Planificador.h"

class ProductorConsumidor {
private:
    std::queue<int> buffer_compartido;
    const size_t CAPACIDAD_MAXIMA = 5;// Capacidad máxima del buffer
public:
    ProductorConsumidor() = default;
    void simularProductor(Planificador& Planificador, GestorComunicacion& ipc, uint32_t pid, int item);
    void simularConsumidor(Planificador& Planificador, GestorComunicacion& ipc, uint32_t pid);
    size_t obtenerTamanioBuffer() const { return buffer_compartido.size(); }
};

#endif // PRODUCTOR_CONSUMIDOR_HPP
