#ifndef PRODUCTOR_CONSUMIDOR_HPP
#define PRODUCTOR_CONSUMIDOR_HPP

#include <queue>
#include <cstdint>
#include <mutex>
#include "../GestorComunicacion/GestorComunicacion.h"
#include "../Planificador/Planificador.h"
#include "../GestorLogs/GestorLogs.h"

class ProductorConsumidor {
private:
    std::queue<int> buffer_compartido;
    const size_t CAPACIDAD_MAXIMA = 5;// Capacidad máxima del buffer
    std::mutex seccion_critica_mtx; // Mutex físico para atomicidad estricta C++
public:
    ProductorConsumidor() = default;
    void simularProductor(Planificador& Planificador, GestorComunicacion& ipc, GestorLogs& logs, uint32_t pid, int item);
    void simularConsumidor(Planificador& Planificador, GestorComunicacion& ipc, GestorLogs& logs, uint32_t pid);
    size_t obtenerTamanioBuffer() const { return buffer_compartido.size(); }
    size_t obtenerCapacidadMaxima() const { return CAPACIDAD_MAXIMA; }
};

#endif // PRODUCTOR_CONSUMIDOR_HPP
