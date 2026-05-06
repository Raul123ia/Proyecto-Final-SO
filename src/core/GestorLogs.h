#ifndef GESTOR_LOGS_H
#define GESTOR_LOGS_H

#include <string>
#include <vector>
#include <cstdint>
#include "CausaTerminacion.h"

class GestorLogs {
private:
    std::vector<std::string> entradas;

public:
    GestorLogs();

    void anotarEvento(std::string descripcion);
    void anotarSalida(uint32_t pid, CausaTerminacion causa);
    std::vector<std::string> exportarLogs();
};

#endif // GESTOR_LOGS_H
#endif // GESTOR_LOGS_H