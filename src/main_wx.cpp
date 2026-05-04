#include <wx/wx.h>
#include "model/Model.h"
#include "controller/Controller.h"
#include "view/InterfazGrafica.h"

namespace sim {

class SimuladorApp : public wxApp {
public:
    virtual bool OnInit() override;
};

bool SimuladorApp::OnInit() {
    Model model;
    Controller* controller = new Controller(model);

    InterfazGrafica* frame = new InterfazGrafica(controller);
    frame->Show();

    return true;
}

} // namespace sim

wxIMPLEMENT_APP(sim::SimuladorApp);

