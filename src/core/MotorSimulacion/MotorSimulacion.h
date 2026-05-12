#pragma once
#include <string>

// Inclusiones de tus módulos homologados
#include "core/Planificador/Planificador.h"
#include "core/GestorComunicacion/GestorComunicacion.h"
#include "core/GestorRecursos/GestorRecursos.h" 
#include "core/GestorLogs/GestorLogs.h"
#include "core/CausaTerminacion/CausaTerminacion.h"
#include "core/ProductorConsumidor/ProductorConsumidor.h"

class MotorSimulacion {
private:
    // El "Hardware" y el "Kernel" bajo el capó
    Planificador planificador;
    GestorComunicacion comunicacion;
    GestorRecursos recursos;
    GestorLogs registros;
    SimulacionIPC simulacionIPC;
public:
    // Constructor
    MotorSimulacion();

    // Métodos que consumirá la Interfaz Gráfica (La API Pública)
    void iniciar(TipoAlgoritmo algoritmo, int quantum);
    // ================= CREAR PROCESO =================
    bool crearProceso(std::string nombre,int rafaga,int prioridad,int memoria);
    void simularProductor(uint32_t pid, int item);
    void simularConsumidor(uint32_t pid);
    // ================= RAM =================
    bool validarMemoriaProceso(uint32_t memoria) const;

    bool asignarMemoriaProceso(uint32_t pid,uint32_t memoria);

    uint32_t liberarMemoriaProceso(uint32_t pid);

    // ================= CPU =================
    bool asignarCPUProceso(uint32_t pid);

    void liberarCPUProceso(uint32_t pid);

    // ================= LIBERAR MEMORIA =================
    void liberarMemoriaRAM(uint32_t pid);

    // ================= GETTERS =================
    uint32_t obtenerMemoriaUsada() const;
    
    // Este es el corazón de tu pipeline de 4 fases
    void ejecutarPasoSiguiente(); 
    // Métodos para controlar procesos (suspender, finalizar, etc.)
    void forzarSalidaProceso(uint32_t pid, CausaTerminacion causa);
    void cambiarEstadoSuspension(uint32_t pid, bool suspender);
    //Metodo para obtener memoria usada, que simplemente delega la consulta al GestorRecursos, asegurando que siempre refleje el estado real del sistema.


    // Getters de solo lectura para que la UI pueda dibujar las tablas
    const Planificador& obtenerPlanificador() const;
    const GestorRecursos& obtenerRecursos() const;
    const GestorLogs& obtenerLogs() const;
    // El puerto de entrada para que las aplicaciones hablen con el Kernel
    bool invocarLlamadaSistema(TipoLlamada tipo, std::string recurso);
};