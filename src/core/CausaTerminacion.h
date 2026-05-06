#ifndef CAUSA_TERMINACION_H
#define CAUSA_TERMINACION_H

// Enumeración que define las causas de finalización de un proceso.
enum class CausaTerminacion {
    NORMAL,
    ERROR,
    INTERBLOQUEO,
    USUARIO
};

#endif // CAUSA_TERMINACION_H