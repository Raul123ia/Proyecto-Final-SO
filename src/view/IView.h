#pragma once

namespace sim {

class IView {
public:
    virtual ~IView() = default;

    // Inicia el bucle de la vista (por ejemplo, lectura de comandos y render)
    virtual void start() = 0;
};

} // namespace sim

