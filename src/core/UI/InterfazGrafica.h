#pragma once

#include <QMainWindow>
#include <QString>
#include <QVector>

class QListWidget;
class QProgressBar;
class QTableWidget;
class QTextEdit;
class QPushButton;

class InterfazGrafica final : public QMainWindow
{
    Q_OBJECT

public:
    explicit InterfazGrafica(QWidget *parent = nullptr);
    ~InterfazGrafica() override = default;

    void mostrarColasPlanificacion();
    void listarProcesosVRecursos();
    void mostrarHistorialLogs();
    void capturarDatosProceso();

public slots:
    void ejecutarAccionUsuario();
    void crearProceso();

private:
    struct ProcesoMock
    {
        int pid{};
        QString nombre;
        QString estado;
        int prioridad{};
        int rafaga{};
        int memoria{};
    };


    void construirInterfaz();
    void conectarSenales();
    void aplicarEstilo();
    void cargarDatosMock();
    void actualizarVistas();
    void registrarEvento(const QString &mensaje);
    int memoriaUsada() const;

    QVector<ProcesoMock> m_procesos;
    QVector<ProcesoMock> m_colaListos;
    QVector<ProcesoMock> m_colaSuspendidos;
    QVector<QString> m_historial;

    QListWidget *m_listaListos{};
    QListWidget *m_listaSuspendidos{};
    QTableWidget *m_tablaProcesos{};
    QTextEdit *m_historialLogs{};
    QProgressBar *m_barraMemoria{};
    QPushButton *m_btnSiguientePaso{};
    QPushButton *m_btnCrearProceso{};
};

