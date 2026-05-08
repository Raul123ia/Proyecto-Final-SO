<<<<<<< HEAD
#include <iostream>

int main() {
    std::cout << "Iniciando Simulador de Gestor de Procesos..." << std::endl;
    std::cout << "Arquitectura base inicializada." << std::endl;
    
    // Aquí inicializarán los módulos: PCB, Planificador, UI, etc.
    
    return 0;
=======
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>

#include "core/UI/InterfazGrafica.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("SimuladorGestorProcesos"));
    app.setApplicationDisplayName(QStringLiteral("Simulador de Gestión de Procesos"));
    app.setOrganizationName(QStringLiteral("CLionProjects"));
    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    InterfazGrafica ventana;
    ventana.show();

    return app.exec();
>>>>>>> origin/UI
}