/*
#ifndef CAUSA_TERMINACION_H
#define CAUSA_TERMINACION_H

#include <string>

enum class CausaTerminacion {

    NORMAL,

    // ================= RAM =================
    MEMORIA_INSUFICIENTE_FAKE,
    FRAGMENTACION_FAKE,

    // ================= CPU =================
    CPU_SATURADA_FAKE,
    QUANTUM_AGOTADO_FAKE
};

class CausaTerminacionManager {

public:

    static std::string obtenerDescripcion(CausaTerminacion causa);

    static std::string obtenerSolucion(CausaTerminacion causa);

};

/#endif // CAUSA_TERMINACION_H
*/