#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>
#include "core/MotorSimulacion/MotorSimulacion.h"
#include "core/UI/InterfazGrafica.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>

// =========================================================================
// PUNTO DE ENTRADA PRINCIPAL (EL "BOOT" DE TU SISTEMA OPERATIVO)
// =========================================================================
int main(int argc, char *argv[])
{
    // 1. Inicialización de Qt (El monitor y los periféricos)
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("SimuladorGestorProcesos"));
    app.setApplicationDisplayName(QStringLiteral("Simulador de Gestión de Procesos"));
    app.setOrganizationName(QStringLiteral("CLionProjects"));
    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    // 2. Encendido del "Hardware" y el "Kernel"
    // Al instanciar el motor, internamente se levantan el Planificador, 
    // el Gestor de Recursos (RAM), el Gestor de Comunicaciones (IPC) y los Logs.
    MotorSimulacion motor;

    // Puedes configurar el estado inicial del SO aquí si lo deseas
    // motor.iniciar(TipoAlgoritmo::ROUND_ROBIN, 3);

    // 3. Inicialización de la Vista (Inyección de Dependencias)
    // Conectamos la interfaz gráfica directamente a los fierros del motor.
    InterfazGrafica ventana(motor);
    ventana.show();

    // 4. Bucle principal
    // A partir de aquí, el simulador avanza impulsado por los eventos de la ventana 
    // (cada vez que presiones el botón "Siguiente Paso" o crees un proceso).
    return app.exec();
}