#ifndef GESTOR_COMUNICACION_HPP
#define GESTOR_COMUNICACION_HPP

#include <iostream>
#include <string>
#include <map>
#include <queue>

class Proceso {
public:
    int pid;
    std::string nombre;
    
    Proceso(int id, std::string nom) : pid(id), nombre(nom) {}
};

class GestorComunicacion {
private:
    std::map<std::string, int> semaforos;
    std::map<std::string, std::queue<Proceso*>> procesos_bloqueados;

public:
    void inicializarSemaforo(std::string nombre_recurso, int permisos_iniciales);
    bool esperarSemaforo(std::string nombre_recurso, Proceso* p);
    Proceso* liberarSemaforo(std::string nombre_recurso);
};

#endif // GESTOR_COMUNICACION_HPP
