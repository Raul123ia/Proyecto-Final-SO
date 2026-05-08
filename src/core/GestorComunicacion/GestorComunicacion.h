#ifndef GESTOR_COMUNICACION_HPP
#define GESTOR_COMUNICACION_HPP

#include <string>
#include <map>
#include <queue>
#include <cstdint> // Necesario para usar uint32_t

class GestorComunicacion {
private:
    std::map<std::string, int> semaforos;
    std::map<std::string, std::queue<uint32_t>> procesos_bloqueados;

public:
    void inicializarSemaforo(std::string nombre_recurso, int permisos_iniciales);
    bool esperarSemaforo(std::string nombre_recurso, uint32_t pid);
    uint32_t liberarSemaforo(std::string nombre_recurso);
};

#endif // GESTOR_COMUNICACION_HPP
