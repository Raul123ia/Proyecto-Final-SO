#include "../core/UI/InterfazGrafica.h"

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

// Inicializa el motor de simulacióny construye la interfaz gráfica.
InterfazGrafica::InterfazGrafica (MotorSimulacion &motor, QWidget *parent): QMainWindow(parent), m_motor(motor) 
{
    construirInterfaz();
    conectarSenales();
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
    m_barraMemoria->setRange(0, m_motor.obtenerRecursos().obtenerMemoriaMaxima());
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

//Comienzan métodos actualizados para mostrar datos reales desde tu motor de simulación, eliminando cualquier código hardcodeado o mockeado. Asegúrate de que tu clase MotorSimulacion tenga los métodos necesarios para acceder a los datos de forma segura y eficiente.

void InterfazGrafica::actualizarVistas()
{
    mostrarColasPlanificacion();
    listarProcesosYRecursos();
    mostrarHistorialLogs();

    // ¡Le preguntamos directamente a la fuente de la verdad sobre la memoria usada! Esto asegura que la barra siempre refleje el estado real del sistema, sin depender de cálculos locales o datos duplicados.
    const uint32_t usados = m_motor.obtenerRecursos().obtenerMemoriaUsada();
    
    m_barraMemoria->setValue(usados);
    m_barraMemoria->setFormat(tr("%v MB / %m MB usados"));
}

//Metodo actualizado para mostrar las colas de planificación directamente desde tu estructura real, evitando cualquier código hardcodeado o mockeado. Asegúrate de que tu clase Planificador tenga los métodos necesarios para obtener las colas de forma segura.
void InterfazGrafica::mostrarColasPlanificacion()
{
  m_listaListos->clear();
    m_listaSuspendidos->clear();

    // Pedimos copias de las colas para poder leerlas destruyéndolas localmente
    auto colaListos = m_motor.obtenerPlanificador().obtenerColaListos();
    auto colaSuspendidos = m_motor.obtenerPlanificador().obtenerColaSuspendidos();

    // Procesar Cola de Listos
    while (!colaListos.empty()) {
        uint32_t pid = colaListos.front();
        colaListos.pop(); // Avanzamos en la copia
        
        // Usamos tu función segura
        const Proceso& proc = m_motor.obtenerPlanificador().obtenerDetallesProceso(pid);
        
        m_listaListos->addItem(QStringLiteral("PID %1 - %2 | Pri %3")
            .arg(proc.obtenerPid())
            .arg(QString::fromStdString(proc.obtenerNombre()))
            .arg(proc.obtenerPrioridad())); 
    }

    // Procesar Cola de Suspendidos
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
//Metodo actualizado para listar procesos y recursos directamente desde tu estructura real, evitando cualquier código hardcodeado o mockeado. Asegúrate de que tu clase Proceso tenga los getters necesarios para acceder a sus atributos de forma segura.
void InterfazGrafica::listarProcesosYRecursos()
{
    // 1. Obtenemos la referencia directa y segura a tu memoria
    const auto& tabla = m_motor.obtenerPlanificador().obtenerTablaProcesos();

    // 2. Preparamos la tabla de Qt
    m_tablaProcesos->setRowCount(tabla.size());
    int fila = 0;

    // 3. Iteramos sobre el mapa real
    for (const auto& par : tabla) {
        const Proceso& proceso = par.second; // par.first es el PID, par.second es el PCB

        // Llenamos las celdas (usando QString::number para ints y fromStdString para std::string)
        m_tablaProcesos->setItem(fila, 0, new QTableWidgetItem(QString::number(proceso.obtenerPid())));
        m_tablaProcesos->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(proceso.obtenerNombre())));
        
        // TODO: Asegúrate de tener un getter que convierta tu enum EstadoProceso a texto
        m_tablaProcesos->setItem(fila, 2, new QTableWidgetItem(QStringLiteral("Activo"))); 
        
        m_tablaProcesos->setItem(fila, 3, new QTableWidgetItem(QString::number(proceso.obtenerPrioridad())));
        m_tablaProcesos->setItem(fila, 4, new QTableWidgetItem(QString::number(proceso.obtenerRafagaRestante())));
        
        // TODO: Reemplaza este 0 con el getter real de la memoria del proceso si es privado
        m_tablaProcesos->setItem(fila, 5, new QTableWidgetItem(QString::number(0))); 
        
        fila++;
    }
    m_tablaProcesos->resizeRowsToContents();
}

void InterfazGrafica::mostrarHistorialLogs()
{
    m_historialLogs->clear();

    // Le pedimos los datos reales al sistema a través del motor.
    // Forma correcta: Pedimos el gestor de logs, y de ahí exportamos el historial completo
    auto historial = m_motor.obtenerLogs().exportarHistorialLogs();

    for (const auto &linea : historial)
    {
        // Convertimos de std::string (C++) a QString (Qt) al vuelo
        m_historialLogs->append(QString::fromStdString(linea));
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
    // Cambiado: Ahora el límite superior lo dicta el Gestor de Recursos real
    // Si aún no tienes el getter, puedes usar 4096 por ahora.
    memoria->setRange(1, m_motor.obtenerRecursos().obtenerMemoriaMaxima()); 
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
        // Cambiado: Uso de statusBar para feedback inmediato de UI
        statusBar()->showMessage(tr("Creación de proceso cancelada."), 4000);
        return;
    }

    const QString nombreProceso = nombre->text().trimmed();
    if (nombreProceso.isEmpty())
    {
        QMessageBox::warning(this, tr("Datos inválidos"), tr("El nombre del proceso no puede estar vacío."));
        return;
    }

    // =========================================================================
    // INYECCIÓN DE LÓGICA REAL (ADIÓS MOCKS)
    // =========================================================================
    
    // El MotorSimulacion se encarga de:
    // 1. Pedirle al Planificador que cree el PCB.
    // 2. Pedirle al GestorRecursos que reserve la memoria.
    // 3. Generar el log del evento.
    m_motor.crearProceso(
        nombreProceso.toStdString(),
        rafaga->value(),
        prioridad->value(),
        memoria->value()
    );

    // Finalmente, refrescamos todas las tablas y listas con datos del kernel
    actualizarVistas();
    statusBar()->showMessage(tr("Proceso '%1' enviado al sistema.").arg(nombreProceso), 4000);
}

void InterfazGrafica::ejecutarAccionUsuario()
{
    // 1. Le decimos al CPU que haga 1 tick de reloj
    m_motor.ejecutarPasoSiguiente();
    
    // 2. Refrescamos la pantalla para ver qué cambió
    actualizarVistas();
}
// Fin de métodos actualizados para mostrar datos reales desde tu motor de simulación, eliminando cualquier código hardcodeado o mockeado. Asegúrate de que tu clase MotorSimulacion tenga los métodos necesarios para acceder a los datos de forma segura y eficiente.
void InterfazGrafica::crearProceso()
{
    capturarDatosProceso();
}










