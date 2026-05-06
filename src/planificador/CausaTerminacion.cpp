#include "CausaTerminacion.h"

std::string causaToString(CausaTerminacion causa) {
    switch (causa) {
        case CausaTerminacion::NORMAL:       return "NORMAL";
        case CausaTerminacion::ERROR:        return "ERROR";
        case CausaTerminacion::INTERBLOQUEO: return "INTERBLOQUEO";
        case CausaTerminacion::USUARIO:      return "USUARIO";
        default:                             return "DESCONOCIDA";
    }
}