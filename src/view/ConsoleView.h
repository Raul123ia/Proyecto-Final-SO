#pragma once

#include "IView.h"
#include "../controller/Controller.h"
#include <memory>

namespace sim {

class ConsoleView : public IView {
public:
    explicit ConsoleView(Controller& controller);
    void start() override;

private:
    Controller& controller_;
    void render();
    void handleCommand(const std::string& cmd);
};

} // namespace sim

