#include "QtView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QString>

namespace sim {

QtView::QtView(Controller& controller, QWidget* parent)
    : QWidget(parent), controller_(controller) {
    auto* vlayout = new QVBoxLayout(this);

    table_ = new QTableWidget(this);
    table_->setColumnCount(6);
    table_->setHorizontalHeaderLabels({"PID", "Name", "State", "Priority", "Burst", "Mem"});
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    vlayout->addWidget(table_);

    auto* hl = new QHBoxLayout();
    btnCreate_ = new QPushButton("Crear proceso", this);
    btnTick_ = new QPushButton("Tick", this);
    btnRefresh_ = new QPushButton("Refrescar", this);

    hl->addWidget(btnCreate_);
    hl->addWidget(btnTick_);
    hl->addWidget(btnRefresh_);
    vlayout->addLayout(hl);

    connect(btnCreate_, &QPushButton::clicked, this, [this]() {
        static int counter = 1;
        controller_.createProcess("proc_gui_" + std::to_string(counter++));
        refresh();
    });

    connect(btnTick_, &QPushButton::clicked, this, [this]() {
        controller_.tick();
        refresh();
    });

    connect(btnRefresh_, &QPushButton::clicked, this, [this]() {
        refresh();
    });

    refresh();
}

void QtView::refresh() {
    const auto& procs = controller_.model().getProcesses();
    table_->setRowCount(static_cast<int>(procs.size()));
    for (int i = 0; i < (int)procs.size(); ++i) {
        const auto& p = procs[i];
        QString estado;
        switch (p.estado) {
            case EstadoProceso::LISTO: estado = "LISTO"; break;
            case EstadoProceso::EJECUTANDO: estado = "EJECUTANDO"; break;
            case EstadoProceso::ESPERANDO: estado = "ESPERANDO"; break;
            case EstadoProceso::TERMINADO: estado = "TERMINADO"; break;
        }
        table_->setItem(i, 0, new QTableWidgetItem(QString::number(p.pid)));
        table_->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(p.nombre)));
        table_->setItem(i, 2, new QTableWidgetItem(estado));
        table_->setItem(i, 3, new QTableWidgetItem(QString::number(p.prioridad)));
        table_->setItem(i, 4, new QTableWidgetItem(QString::number(p.rafaga_restante)));
        table_->setItem(i, 5, new QTableWidgetItem(QString::number(p.memoria_asignada)));
    }
}

} // namespace sim

