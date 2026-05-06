#include "GestorLogs.h"
#include <iostream>

GestorLogs::GestorLogs() {
}

void GestorLogs::anotarEvento(std::string descripcion) {
    entradas.push_back(descripcion);
}

void GestorLogs::anotarSalida(uint32_t pid, CausaTerminacion causa) {
    // Como quitamos causaToString() para respetar el diagrama,
    // resolvemos la conversión internamente dentro de la implementación.
    std::string causaStr = "DESCONOCIDA";
    switch (causa) {
        case CausaTerminacion::NORMAL:       causaStr = "NORMAL"; break;
        case CausaTerminacion::ERROR:        causaStr = "ERROR"; break;
        case CausaTerminacion::INTERBLOQUEO: causaStr = "INTERBLOQUEO"; break;
        case CausaTerminacion::USUARIO:      causaStr = "USUARIO"; break;
    }

    std::string log = "Proceso " + std::to_string(pid) + " finalizado. Causa: " + causaStr;
    entradas.push_back(log);
}

std::vector<std::string> GestorLogs::exportarLogs() {
    return entradas;
}