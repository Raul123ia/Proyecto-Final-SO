#include "core/InterfazGrafica.h"

#include <QAbstractItemView>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QStatusBar>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QVBoxLayout>

namespace
{
constexpr int kMemoriaTotalMb = 4096;
}

InterfazGrafica::InterfazGrafica(QWidget *parent)
    : QMainWindow(parent)
{
    construirInterfaz();
    conectarSenales();
    cargarDatosMock();
    aplicarEstilo();
    actualizarVistas();
}

void InterfazGrafica::construirInterfaz()
{
    auto *central = new QWidget(this);
    auto *raiz = new QVBoxLayout(central);
    raiz->setContentsMargins(16, 16, 16, 16);
    raiz->setSpacing(12);

    auto *encabezado = new QHBoxLayout();
    auto *titulo = new QLabel(tr("Simulador de Gestión de Procesos"), this);
    titulo->setObjectName("tituloPrincipal");
    auto *subtitulo = new QLabel(tr("Interfaz de monitoreo y control - Qt6"), this);
    subtitulo->setObjectName("subtituloPrincipal");
    auto *contenedorTitulo = new QVBoxLayout();
    contenedorTitulo->addWidget(titulo);
    contenedorTitulo->addWidget(subtitulo);
    encabezado->addLayout(contenedorTitulo);
    encabezado->addStretch();

    m_barraMemoria = new QProgressBar(this);
    m_barraMemoria->setObjectName("barraMemoria");
    m_barraMemoria->setRange(0, kMemoriaTotalMb);
    m_barraMemoria->setFormat(tr("%v MB / %m MB"));
    m_barraMemoria->setAlignment(Qt::AlignCenter);
    m_barraMemoria->setTextVisible(true);
    m_barraMemoria->setMinimumWidth(280);
    encabezado->addWidget(m_barraMemoria);

    raiz->addLayout(encabezado);

    auto *zonaBotones = new QHBoxLayout();
    m_btnSiguientePaso = new QPushButton(tr("Siguiente Paso"), this);
    m_btnSiguientePaso->setObjectName("btnPrincipal");
    m_btnCrearProceso = new QPushButton(tr("Crear Proceso"), this);
    m_btnCrearProceso->setObjectName("btnSecundario");
    zonaBotones->addWidget(m_btnSiguientePaso);
    zonaBotones->addWidget(m_btnCrearProceso);
    zonaBotones->addStretch();
    raiz->addLayout(zonaBotones);

    auto *paneles = new QHBoxLayout();
    paneles->setSpacing(12);

    auto *panelColas = new QWidget(this);
    panelColas->setObjectName("panelTarjeta");
    auto *layoutColas = new QVBoxLayout(panelColas);
    auto *lblColas = new QLabel(tr("Colas de Planificación"), panelColas);
    lblColas->setObjectName("tituloPanel");
    layoutColas->addWidget(lblColas);

    auto *subgridColas = new QHBoxLayout();
    auto *panelListos = new QVBoxLayout();
    auto *lblListos = new QLabel(tr("Cola de Listos"), panelColas);
    lblListos->setObjectName("subtituloPanel");
    m_listaListos = new QListWidget(panelColas);
    m_listaListos->setSelectionMode(QAbstractItemView::SingleSelection);
    panelListos->addWidget(lblListos);
    panelListos->addWidget(m_listaListos);

    auto *panelSuspendidos = new QVBoxLayout();
    auto *lblSuspendidos = new QLabel(tr("Cola de Suspendidos"), panelColas);
    lblSuspendidos->setObjectName("subtituloPanel");
    m_listaSuspendidos = new QListWidget(panelColas);
    m_listaSuspendidos->setSelectionMode(QAbstractItemView::SingleSelection);
    panelSuspendidos->addWidget(lblSuspendidos);
    panelSuspendidos->addWidget(m_listaSuspendidos);

    subgridColas->addLayout(panelListos);
    subgridColas->addLayout(panelSuspendidos);
    layoutColas->addLayout(subgridColas);

    auto *panelTabla = new QWidget(this);
    panelTabla->setObjectName("panelTarjeta");
    auto *layoutTabla = new QVBoxLayout(panelTabla);
    auto *lblTabla = new QLabel(tr("Procesos y Recursos"), panelTabla);
    lblTabla->setObjectName("tituloPanel");
    layoutTabla->addWidget(lblTabla);
    m_tablaProcesos = new QTableWidget(panelTabla);
    m_tablaProcesos->setColumnCount(6);
    m_tablaProcesos->setHorizontalHeaderLabels({tr("PID"), tr("Nombre"), tr("Estado"), tr("Prioridad"), tr("Ráfaga"), tr("Memoria (MB)")});
    m_tablaProcesos->horizontalHeader()->setStretchLastSection(true);
    m_tablaProcesos->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tablaProcesos->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tablaProcesos->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tablaProcesos->setAlternatingRowColors(true);
    layoutTabla->addWidget(m_tablaProcesos);

    auto *panelLogs = new QWidget(this);
    panelLogs->setObjectName("panelTarjeta");
    auto *layoutLogs = new QVBoxLayout(panelLogs);
    auto *lblLogs = new QLabel(tr("Historial de Eventos"), panelLogs);
    lblLogs->setObjectName("tituloPanel");
    layoutLogs->addWidget(lblLogs);
    m_historialLogs = new QTextEdit(panelLogs);
    m_historialLogs->setReadOnly(true);
    m_historialLogs->setMinimumHeight(160);
    layoutLogs->addWidget(m_historialLogs);

    auto *columnaIzquierda = new QVBoxLayout();
    columnaIzquierda->addWidget(panelColas, 1);
    columnaIzquierda->addWidget(panelLogs, 1);

    paneles->addLayout(columnaIzquierda, 1);
    paneles->addWidget(panelTabla, 2);

    raiz->addLayout(paneles, 1);
    setCentralWidget(central);
    resize(1280, 780);
    setWindowTitle(tr("Simulador de Gestión de Procesos"));
    statusBar()->showMessage(tr("Listo para simular acciones"));
}

void InterfazGrafica::conectarSenales()
{
    connect(m_btnSiguientePaso, &QPushButton::clicked, this, &InterfazGrafica::ejecutarAccionUsuario);
    connect(m_btnCrearProceso, &QPushButton::clicked, this, &InterfazGrafica::crearProceso);
}

void InterfazGrafica::aplicarEstilo()
{
    setStyleSheet(QStringLiteral(R"(
        QMainWindow {
            background: %1;
            color: %2;
        }
        QWidget#panelTarjeta {
            background: %3;
            border: 1px solid rgba(255, 255, 255, 0.08);
            border-radius: 14px;
        }
        QLabel#tituloPrincipal {
            color: %2;
            font-size: 24px;
            font-weight: 700;
        }
        QLabel#subtituloPrincipal {
            color: rgba(229, 231, 235, 0.72);
            font-size: 12px;
        }
        QLabel#tituloPanel {
            color: %2;
            font-size: 16px;
            font-weight: 600;
            padding: 2px 0;
        }
        QLabel#subtituloPanel {
            color: rgba(229, 231, 235, 0.72);
            font-size: 12px;
        }
        QListWidget, QTableWidget, QTextEdit {
            background: rgba(17, 24, 39, 0.92);
            color: %2;
            border: 1px solid rgba(255, 255, 255, 0.08);
            border-radius: 10px;
            selection-background-color: %4;
            selection-color: white;
        }
        QHeaderView::section {
            background: rgba(47, 128, 237, 0.14);
            color: %2;
            border: none;
            padding: 8px;
            font-weight: 600;
        }
        QPushButton {
            color: white;
            border: none;
            border-radius: 10px;
            padding: 10px 18px;
            font-weight: 600;
        }
        QPushButton#btnPrincipal {
            background: %4;
        }
        QPushButton#btnSecundario {
            background: rgba(255, 255, 255, 0.12);
        }
        QPushButton:hover {
            background-color: rgba(47, 128, 237, 0.78);
        }
        QProgressBar {
            background: rgba(17, 24, 39, 0.92);
            color: %2;
            border: 1px solid rgba(255, 255, 255, 0.08);
            border-radius: 10px;
            text-align: center;
            padding: 2px;
        }
        QProgressBar::chunk {
            border-radius: 8px;
            background-color: %4;
        }
    )")
            .arg(QStringLiteral("#0F172A"), QStringLiteral("#E5E7EB"), QStringLiteral("#111827"), QStringLiteral("#2F80ED")));
}

void InterfazGrafica::cargarDatosMock()
{
    m_procesos = {
        {101, tr("Navegador"), tr("Listo"), 2, 18, 640},
        {102, tr("Editor"), tr("Listo"), 3, 11, 512},
        {103, tr("Compilador"), tr("Suspendido"), 1, 26, 896},
        {104, tr("Terminal"), tr("Ejecución"), 0, 8, 128},
        {105, tr("ServicioBackup"), tr("Suspendido"), 4, 42, 704},
        {106, tr("Monitor"), tr("Listo"), 1, 7, 256}
    };

    m_colaListos.clear();
    m_colaSuspendidos.clear();

    for (const auto &proceso : m_procesos)
    {
        if (proceso.estado.compare(tr("Suspendido"), Qt::CaseInsensitive) == 0)
        {
            m_colaSuspendidos.push_back(proceso);
        }
        else if (proceso.estado.compare(tr("Listo"), Qt::CaseInsensitive) == 0)
        {
            m_colaListos.push_back(proceso);
        }
    }

    m_historial = {
        tr("[Sistema] Inicialización de la interfaz completada."),
        tr("[Sistema] Cargando datos de prueba de procesos."),
        tr("[CPU] Cola de planificación preparada para visualización.")
    };
}

void InterfazGrafica::actualizarVistas()
{
    mostrarColasPlanificacion();
    listarProcesosVRecursos();
    mostrarHistorialLogs();

    const int usados = memoriaUsada();
    m_barraMemoria->setValue(usados);
    m_barraMemoria->setFormat(tr("%v MB / %m MB usados"));
}

void InterfazGrafica::mostrarColasPlanificacion()
{
    m_listaListos->clear();
    m_listaSuspendidos->clear();

    for (const auto &proceso : m_colaListos)
    {
        m_listaListos->addItem(QStringLiteral("PID %1 - %2 | Pri %3 | %4 MB")
                                   .arg(proceso.pid)
                                   .arg(proceso.nombre)
                                   .arg(proceso.prioridad)
                                   .arg(proceso.memoria));
    }

    for (const auto &proceso : m_colaSuspendidos)
    {
        m_listaSuspendidos->addItem(QStringLiteral("PID %1 - %2 | Pri %3 | %4 MB")
                                        .arg(proceso.pid)
                                        .arg(proceso.nombre)
                                        .arg(proceso.prioridad)
                                        .arg(proceso.memoria));
    }
}

void InterfazGrafica::listarProcesosVRecursos()
{
    m_tablaProcesos->setRowCount(m_procesos.size());

    for (int fila = 0; fila < m_procesos.size(); ++fila)
    {
        const auto &proceso = m_procesos.at(fila);
        m_tablaProcesos->setItem(fila, 0, new QTableWidgetItem(QString::number(proceso.pid)));
        m_tablaProcesos->setItem(fila, 1, new QTableWidgetItem(proceso.nombre));
        m_tablaProcesos->setItem(fila, 2, new QTableWidgetItem(proceso.estado));
        m_tablaProcesos->setItem(fila, 3, new QTableWidgetItem(QString::number(proceso.prioridad)));
        m_tablaProcesos->setItem(fila, 4, new QTableWidgetItem(QString::number(proceso.rafaga)));
        m_tablaProcesos->setItem(fila, 5, new QTableWidgetItem(QString::number(proceso.memoria)));
    }

    m_tablaProcesos->resizeRowsToContents();
}

void InterfazGrafica::mostrarHistorialLogs()
{
    m_historialLogs->clear();
    for (const auto &linea : m_historial)
    {
        m_historialLogs->append(linea);
    }
}

void InterfazGrafica::capturarDatosProceso()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Crear nuevo proceso"));
    dialog.setModal(true);

    auto *layout = new QVBoxLayout(&dialog);
    auto *form = new QFormLayout();

    auto *nombre = new QLineEdit(&dialog);
    nombre->setPlaceholderText(tr("Ej: Servidor"));

    auto *rafaga = new QSpinBox(&dialog);
    rafaga->setRange(1, 100000);
    rafaga->setValue(10);

    auto *prioridad = new QSpinBox(&dialog);
    prioridad->setRange(0, 9);
    prioridad->setValue(3);

    auto *memoria = new QSpinBox(&dialog);
    memoria->setRange(1, kMemoriaTotalMb);
    memoria->setValue(256);

    form->addRow(tr("Nombre"), nombre);
    form->addRow(tr("Ráfaga"), rafaga);
    form->addRow(tr("Prioridad"), prioridad);
    form->addRow(tr("Memoria (MB)"), memoria);
    layout->addLayout(form);

    auto *botones = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(botones);

    connect(botones, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(botones, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted)
    {
        registrarEvento(tr("[Usuario] Creación de proceso cancelada."));
        return;
    }

    const QString nombreProceso = nombre->text().trimmed();
    if (nombreProceso.isEmpty())
    {
        QMessageBox::warning(this, tr("Datos inválidos"), tr("El nombre del proceso no puede estar vacío."));
        return;
    }

    const int siguientePid = m_procesos.isEmpty() ? 1 : (m_procesos.constLast().pid + 1);
    ProcesoMock nuevo{ siguientePid, nombreProceso, tr("Listo"), prioridad->value(), rafaga->value(), memoria->value() };

    m_procesos.push_back(nuevo);
    m_colaListos.push_back(nuevo);
    registrarEvento(tr("[Usuario] Proceso '%1' creado con PID %2.").arg(nuevo.nombre).arg(nuevo.pid));
    actualizarVistas();
}

void InterfazGrafica::ejecutarAccionUsuario()
{
    if (m_colaListos.isEmpty())
    {
        registrarEvento(tr("[CPU] No hay procesos listos para ejecutar."));
        QMessageBox::information(this, tr("Sin procesos"), tr("La cola de listos está vacía."));
        return;
    }

    auto proceso = m_colaListos.takeFirst();
    proceso.estado = tr("Finalizado");

    for (auto &item : m_procesos)
    {
        if (item.pid == proceso.pid)
        {
            item.estado = proceso.estado;
            break;
        }
    }

    registrarEvento(tr("[CPU] PID %1 (%2) avanzó al siguiente paso y quedó %3.")
                        .arg(proceso.pid)
                        .arg(proceso.nombre)
                        .arg(proceso.estado));

    actualizarVistas();
}

void InterfazGrafica::crearProceso()
{
    capturarDatosProceso();
}

void InterfazGrafica::registrarEvento(const QString &mensaje)
{
    m_historial.push_back(mensaje);
    mostrarHistorialLogs();
    statusBar()->showMessage(mensaje, 4000);
}

int InterfazGrafica::memoriaUsada() const
{
    int total = 0;
    for (const auto &proceso : m_procesos)
    {
        total += proceso.memoria;
    }
    return qBound(0, total, kMemoriaTotalMb);
}







