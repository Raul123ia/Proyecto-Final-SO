#include "ConsoleView.h"
#include <iostream>

namespace sim {

ConsoleView::ConsoleView(Controller& controller)
    : controller_(controller) {}

void ConsoleView::start() {
    std::cout << "Simulador - Vista de consola\n";
    std::cout << "Comandos: (c) crear proceso, (t) tick, (l) listar, (q) salir\n";
    while (true) {
        std::cout << "> ";
        std::string cmd;
        if (!std::getline(std::cin, cmd)) break;
        if (cmd.empty()) continue;
        handleCommand(cmd);
        if (cmd == "q") break;
    }
    std::cout << "Saliendo de la vista...\n";
}

void ConsoleView::render() {
    const auto& procs = controller_.model().getProcesses();
    std::cout << "--- Estado del sistema ---\n";
    if (procs.empty()) {
        std::cout << "(sin procesos)\n";
        return;
    }
    std::cout << "PID\tName\tState\tPriority\tBurst\tMem\n";
    for (const auto& p : procs) {
        auto estado = "?";
        switch (p.estado) {
            case EstadoProceso::LISTO: estado = "LISTO"; break;
            case EstadoProceso::EJECUTANDO: estado = "EJECUTANDO"; break;
            case EstadoProceso::ESPERANDO: estado = "ESPERANDO"; break;
            case EstadoProceso::TERMINADO: estado = "TERMINADO"; break;
        }
        std::cout << p.pid << "\t" << p.nombre << "\t" << estado << "\t" << p.prioridad << "\t" << p.rafaga_restante << "\t" << p.memoria_asignada << "\n";
    }
}

void ConsoleView::handleCommand(const std::string& cmd) {
    if (cmd == "c") {
        static int counter = 1;
        // valores por defecto para demo
        controller_.crearProceso("proc_" + std::to_string(counter++), 5, 1, 64);
        std::cout << "Proceso creado.\n";
        render();
    } else if (cmd == "t") {
        controller_.ejecutarPasoSiguiente();
        std::cout << "Tick ejecutado.\n";
        render();
    } else if (cmd == "l") {
        render();
    } else if (cmd == "q") {
        // salir: handled by start loop
    } else {
        std::cout << "Comando no reconocido. Usa c/t/l/q.\n";
    }
}

} // namespace sim

