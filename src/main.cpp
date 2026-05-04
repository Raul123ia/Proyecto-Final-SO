#include <iostream>
#include "model/Model.h"
#include "controller/Controller.h"
#include "view/ConsoleView.h"

int main() {
    using namespace sim;
    std::cout << "Iniciando Simulador de Gestor de Procesos..." << std::endl;

    Model model;
    Controller controller(model);
    ConsoleView view(controller);

    // Para demo, crear un par de procesos iniciales
    controller.crearProceso("init", 10, 1, 128);
    controller.crearProceso("worker", 7, 2, 64);

    view.start();

    return 0;
}