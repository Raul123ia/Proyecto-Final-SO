#pragma once

#include <QMainWindow>
#include <QString>
#include <QVector>
#include "core/MotorSimulacion/MotorSimulacion.h"

class QListWidget;
class QProgressBar;
class QTableWidget;
class QTextEdit;
class QPushButton;

class InterfazGrafica final : public QMainWindow
{
    Q_OBJECT

public:
    explicit InterfazGrafica(MotorSimulacion& motor, QWidget *parent = nullptr);
    
    ~InterfazGrafica() override = default;

    void mostrarColasPlanificacion();
    void listarProcesosYRecursos();
    void mostrarHistorialLogs();
    void capturarDatosProceso();

public slots:
    void ejecutarAccionUsuario();
    void crearProceso();

private:
    MotorSimulacion& m_motor;

    void construirInterfaz();
    void conectarSenales();
    void aplicarEstilo();
    void actualizarVistas();
    void registrarEvento(const QString &mensaje);
    

  //  Variables para almacenar referencias a los widgets clave de la interfaz, para poder actualizarlos dinámicamente.

    QListWidget *m_listaListos{};
    QListWidget *m_listaSuspendidos{};
    QTableWidget *m_tablaProcesos{};
    QTextEdit *m_historialLogs{};
    QProgressBar *m_barraMemoria{};
    QPushButton *m_btnSiguientePaso{};
    QPushButton *m_btnCrearProceso{};
};

