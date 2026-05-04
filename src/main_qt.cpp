#include <QApplication>
#include "model/Model.h"
#include "controller/Controller.h"
#include "view/QtView.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    using namespace sim;

    Model model;
    Controller controller(model);
    QtView view(controller);
    view.setWindowTitle("Simulador - GUI");
    view.resize(600, 400);
    view.show();

    return app.exec();
}

