#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <memory>
#include "../controller/Controller.h"

namespace sim {

class QtView : public QWidget {
public:
    explicit QtView(Controller& controller, QWidget* parent = nullptr);
    void refresh();

private:
    Controller& controller_;
    QTableWidget* table_ = nullptr;
    QPushButton* btnCreate_ = nullptr;
    QPushButton* btnTick_ = nullptr;
    QPushButton* btnRefresh_ = nullptr;
};

} // namespace sim

