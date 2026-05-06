/*
//#include "../core/CausaTerminacion/CausaTerminacion.h"

//std::string CausaTerminacionManager::obtenerDescripcion(CausaTerminacion causa) {

    //switch (causa) {

        // ================= NORMAL =================

        //case CausaTerminacion::NORMAL:
            //return "Proceso finalizado correctamente.";

            // ================= RAM =================

        //case CausaTerminacion::MEMORIA_INSUFICIENTE_FAKE:
            //return "Fallo artificial: memoria insuficiente durante asignacion.";

        //case CausaTerminacion::FRAGMENTACION_FAKE:
            //return "Fallo artificial: fragmentacion de memoria detectada.";

            // ================= CPU =================

        //case CausaTerminacion::CPU_SATURADA_FAKE:
            //return "Fallo artificial: CPU saturada temporalmente.";

        //case CausaTerminacion::QUANTUM_AGOTADO_FAKE:
            //return "Fallo artificial: quantum de CPU agotado.";

        //default:
            //return "Causa desconocida.";
    //}
//}

//std::string CausaTerminacionManager::obtenerSolucion(CausaTerminacion causa) {

    //switch (causa) {

        // ================= NORMAL =================

        //case CausaTerminacion::NORMAL:
            //return "No se requiere reparacion.";

            // ================= RAM =================

        //case CausaTerminacion::MEMORIA_INSUFICIENTE_FAKE:
            return "Liberando memoria temporal y reintentando asignacion.";

        case CausaTerminacion::FRAGMENTACION_FAKE:
            return "Compactando memoria y reorganizando bloques.";

            // ================= CPU =================

        case CausaTerminacion::CPU_SATURADA_FAKE:
            return "Esperando liberacion de CPU y reintentando.";

        case CausaTerminacion::QUANTUM_AGOTADO_FAKE:
            return "Reasignando quantum y devolviendo proceso a cola.";

        default:
            return "Sin solucion definida.";
    }
}
