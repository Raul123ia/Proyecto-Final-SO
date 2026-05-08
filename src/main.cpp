
#include <iostream>

int main() {
    std::cout << "Iniciando Simulador de Gestor de Procesos..." << std::endl;
    std::cout << "Arquitectura base inicializada." << std::endl;
    
    // Aquí inicializarán los módulos: PCB, Planificador, UI, etc.
    
    return 0;
}
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>

// Encabezado de la interfaz de tu compañero
#include "core/UI/InterfazGrafica.h"

// TODO: Descomentar estos encabezados cuando homologues las rutas exactas
// #include "core/planificador/Planificador.h"
// #include "core/ipc/GestorComunicacion.h"

// =========================================================================
// MOTOR DEL SIMULADOR: PIPELINE DE 4 FASES (CICLO DE MÁQUINA)
// =========================================================================
// Esta función encapsula la lógica de bajo nivel. 
// La UI deberá llamarla cada vez que el usuario presione "Siguiente Tick" 
// o mediante un QTimer para que corra en automático.
/* void simularUnTick(Planificador& kernel, GestorComunicacion& ipc) {
    // FASE 1: EVENTOS E INTERRUPCIONES (Syscalls)
    // Simulamos si el proceso actual requiere un recurso IPC
    if (kernel.obtenerPidEnEjecucion() != 0) {
        // kernel.llamadaAlSistema(TipoLlamada::WAIT_SEMAFORO, "Mutex", ipc);
    }

    // FASE 2: PLANIFICACIÓN Y DESPACHO
    // El OS decide quién entra a la CPU
    kernel.ejecutarDespachador();

    // FASE 3: EJECUCIÓN DE HARDWARE (CPU)
    // La CPU ejecuta instrucciones
    kernel.ejecutarCPU();

    // FASE 4: CRONÓMETRO GLOBAL
    // El ciclo termina y el tiempo avanza
    kernel.avanzarTiempo();
}
*/

// =========================================================================
// PUNTO DE ENTRADA PRINCIPAL
// =========================================================================
int main(int argc, char *argv[])
{
    // 1. Inicialización de Qt (Código de tu compañero)
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("SimuladorGestorProcesos"));
    app.setApplicationDisplayName(QStringLiteral("Simulador de Gestión de Procesos"));
    app.setOrganizationName(QStringLiteral("CLionProjects"));
    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    // 2. Inicialización del Hardware y el Kernel (Tu arquitectura)
    // Aquí instanciarás tus clases cuando estén listas:
    // Planificador kernel(TipoAlgoritmo::ROUND_ROBIN, 3);
    // GestorComunicacion ipc;

    // 3. Inicialización de la Vista
    // En el futuro, lo ideal es pasarle el kernel a la ventana por referencia:
    // InterfazGrafica ventana(kernel, ipc);
    InterfazGrafica ventana;
    ventana.show();

    // 4. Delegar el control de la ejecución a Qt
    // A partir de aquí, el simulador avanza según los eventos de la ventana
    return app.exec();
}