#include "../core/UI/InterfazGrafica.h"
#include "core/Planificador/Planificador.h"
#include <QAbstractItemView>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QButtonGroup>
#include <QMessageBox>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QStatusBar>
#include <QSpinBox>
#include <QRadioButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QTimer>

// Inicializa el motor de simulación y construye la interfaz gráfica.
InterfazGrafica::InterfazGrafica (MotorSimulacion &motor, QWidget *parent): QMainWindow(parent), m_motor(motor) 
{
    construirInterfaz();
    conectarSenales();
    aplicarEstilo();
    actualizarVistas();
}

void InterfazGrafica::construirInterfaz()
{
    m_relojSistema = new QTimer(this);
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
    m_barraMemoria->setRange(0, m_motor.obtenerRecursos().obtenerMemoriaMaxima());
    m_barraMemoria->setFormat(tr("%v MB / %m MB"));
    m_barraMemoria->setAlignment(Qt::AlignCenter);
    m_barraMemoria->setTextVisible(true);
    m_barraMemoria->setMinimumWidth(280);
    encabezado->addWidget(m_barraMemoria);

    m_labelBuffer = new QLabel(this);
    m_labelBuffer->setObjectName("subtituloPrincipal");
    m_labelBuffer->setText(tr("Buffer: 0/0"));
    encabezado->addWidget(m_labelBuffer);

    m_lblEsperandoMemoria = new QLabel(this);
    m_lblEsperandoMemoria->setObjectName("subtituloPrincipal");
    m_lblEsperandoMemoria->setText(tr("Esperando Memoria: 0"));
    m_lblEsperandoMemoria->setStyleSheet("color: #F59E0B; font-weight: bold;"); 
    encabezado->addWidget(m_lblEsperandoMemoria);
    raiz->addLayout(encabezado);

    auto *zonaBotones = new QHBoxLayout();
    
    // Solo dejamos el botón de Iniciar y el de Algoritmo
    m_btnIniciarSimulacion = new QPushButton(tr("Iniciar Simulacion Automática"), this);
    m_btnIniciarSimulacion->setObjectName("btnPrincipal"); // Lo hacemos el botón principal
    
    auto *btnAlgoritmo = new QPushButton(tr("Seleccionar Algoritmo"), this);
    btnAlgoritmo->setObjectName("btnSecundario");
    
    connect(btnAlgoritmo, &QPushButton::clicked, this, [this, btnAlgoritmo]() {
        QDialog dialog(this);
        dialog.setWindowTitle(tr("Seleccionar algoritmo de planificación"));
        dialog.setModal(true);

        auto *layout = new QVBoxLayout(&dialog);
        auto *descripcion = new QLabel(tr("Elige el algoritmo que se mostrará en la interfaz."), &dialog);
        descripcion->setWordWrap(true);
        layout->addWidget(descripcion);

        auto *grupo = new QButtonGroup(&dialog);
        auto *opFcfs = new QRadioButton(tr("FCFS / FIFO"), &dialog);
        auto *opSjf = new QRadioButton(tr("SJF"), &dialog);
        auto *opPrioridad = new QRadioButton(tr("Prioridad"), &dialog);
        auto *opRR = new QRadioButton(tr("Round Robin"), &dialog);

        grupo->addButton(opFcfs, 0);
        grupo->addButton(opSjf, 1);
        grupo->addButton(opPrioridad, 2);
        grupo->addButton(opRR, 3);

        opFcfs->setChecked(true);

        layout->addWidget(opFcfs);
        layout->addWidget(opSjf);
        layout->addWidget(opPrioridad);
        layout->addWidget(opRR);

        auto *botones = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
        layout->addWidget(botones);

        connect(botones, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(botones, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted) {
            QString algoritmoSeleccionado = tr("FCFS / FIFO");
            if (opSjf->isChecked()) {
                algoritmoSeleccionado = tr("SJF");
            } else if (opPrioridad->isChecked()) {
                algoritmoSeleccionado = tr("Prioridad");
            } else if (opRR->isChecked()) {
                algoritmoSeleccionado = tr("Round Robin");
            }

            btnAlgoritmo->setText(tr("Algoritmo: %1").arg(algoritmoSeleccionado));
            statusBar()->showMessage(tr("Algoritmo seleccionado: %1").arg(algoritmoSeleccionado), 4000);
        }
    });
    
    zonaBotones->addWidget(m_btnIniciarSimulacion);
    zonaBotones->addWidget(btnAlgoritmo);
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
    auto *lblLogs = new QLabel(tr("Historial de Eventos y Logs"), panelLogs);
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
    statusBar()->showMessage(tr("Listo para iniciar la simulación"));
}

void InterfazGrafica::conectarSenales()
{
    // Solo quedan dos conexiones: el botón de iniciar y el reloj automático
    connect(m_btnIniciarSimulacion, &QPushButton::clicked, this, &InterfazGrafica::iniciarSimulacionAutomatica);
    connect(m_relojSistema, &QTimer::timeout, this, &InterfazGrafica::ejecutarAccionUsuario);
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

void InterfazGrafica::actualizarVistas()
{
    mostrarColasPlanificacion();
    listarProcesosYRecursos();
    mostrarHistorialLogs();

    const uint32_t usados = m_motor.obtenerRecursos().obtenerMemoriaUsada();
    m_barraMemoria->setValue(usados);
    m_barraMemoria->setFormat(tr("%v MB / %m MB usados"));

    const size_t ocupacion = m_motor.obtenerOcupacionBuffer();
    const size_t capacidad = m_motor.obtenerCapacidadBuffer();
    m_labelBuffer->setText(tr("Buffer: %1/%2").arg(ocupacion).arg(capacidad));

    const size_t enEspera = m_motor.obtenerProcesosNuevosPendientes();
    m_lblEsperandoMemoria->setText(tr("Esperando Memoria: %1 procesos").arg(enEspera));
}

void InterfazGrafica::mostrarColasPlanificacion()
{
    m_listaListos->clear();
    m_listaSuspendidos->clear();

    auto colaListos = m_motor.obtenerPlanificador().obtenerColaListos();
    auto colaSuspendidos = m_motor.obtenerPlanificador().obtenerColaSuspendidos();

    while (!colaListos.empty()) {
        uint32_t pid = colaListos.front();
        colaListos.pop(); 
        
        const Proceso& proc = m_motor.obtenerPlanificador().obtenerDetallesProceso(pid);
        
        m_listaListos->addItem(QStringLiteral("PID %1 - %2 | Pri %3")
            .arg(proc.obtenerPid())
            .arg(QString::fromStdString(proc.obtenerNombre()))
            .arg(proc.obtenerPrioridad())); 
    }

    while (!colaSuspendidos.empty()) {
        uint32_t pid = colaSuspendidos.front();
        colaSuspendidos.pop();
        
        const Proceso& proc = m_motor.obtenerPlanificador().obtenerDetallesProceso(pid);
        
        m_listaSuspendidos->addItem(QStringLiteral("PID %1 - %2 | Pri %3")
            .arg(proc.obtenerPid())
            .arg(QString::fromStdString(proc.obtenerNombre()))
            .arg(proc.obtenerPrioridad()));
    }
}

void InterfazGrafica::listarProcesosYRecursos()
{
    const auto& tabla = m_motor.obtenerPlanificador().obtenerTablaProcesos();
    m_tablaProcesos->setRowCount(tabla.size());
    int fila = 0;

    for (const auto& par : tabla) {
        const Proceso& proceso = par.second; 

        m_tablaProcesos->setItem(fila, 0, new QTableWidgetItem(QString::number(proceso.obtenerPid())));
        m_tablaProcesos->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(proceso.obtenerNombre())));
        m_tablaProcesos->setItem(fila, 2, new QTableWidgetItem(QStringLiteral("Activo"))); 
        m_tablaProcesos->setItem(fila, 3, new QTableWidgetItem(QString::number(proceso.obtenerPrioridad())));
        m_tablaProcesos->setItem(fila, 4, new QTableWidgetItem(QString::number(proceso.obtenerRafagaRestante())));
        m_tablaProcesos->setItem(fila, 5, new QTableWidgetItem(QString::number(proceso.obtenerMemoriaAsignada())));
        fila++;
    }
    m_tablaProcesos->resizeRowsToContents();
}

void InterfazGrafica::mostrarHistorialLogs()
{
    m_historialLogs->clear();
    auto historial = m_motor.obtenerLogs().exportarHistorialLogs();
    for (const auto &linea : historial)
    {
        m_historialLogs->append(QString::fromStdString(linea));
    }
}

void InterfazGrafica::ejecutarAccionUsuario()
{
    // 1. Le decimos al CPU que haga 1 tick de reloj
    m_motor.ejecutarPasoSiguiente();
    
    // 2. Refrescamos la pantalla
    actualizarVistas();
}

void InterfazGrafica::iniciarSimulacionAutomatica()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Configuración de Simulación Autónoma"));
    dialog.setModal(true);

    auto *layout = new QVBoxLayout(&dialog);
    auto *form = new QFormLayout();

    auto *spinCantidad = new QSpinBox(&dialog);
    spinCantidad->setRange(1, 500);
    spinCantidad->setValue(80); 

    auto *spinVelocidad = new QSpinBox(&dialog);
    spinVelocidad->setRange(10, 5000);
    spinVelocidad->setValue(500); 
    spinVelocidad->setSuffix(" ms");

    form->addRow(tr("Cantidad de procesos a generar:"), spinCantidad);
    form->addRow(tr("Velocidad del reloj (Tick):"), spinVelocidad);
    layout->addLayout(form);

    auto *botones = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(botones);

    connect(botones, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(botones, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        statusBar()->showMessage(tr("Inicio de simulacion cancelado."), 4000);
        return;
    }

    m_btnIniciarSimulacion->setEnabled(false);

    int cantidad = spinCantidad->value();
    m_motor.inicializarSimulacionAutomatica(cantidad);

    int velocidadMs = spinVelocidad->value();
    m_relojSistema->start(velocidadMs);

    actualizarVistas();
    statusBar()->showMessage(tr("Simulación autónoma iniciada a %1 ms por tick.").arg(velocidadMs), 4000);
}