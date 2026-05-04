#include "InterfazGrafica.h"
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <sstream>

namespace sim {

wxBEGIN_EVENT_TABLE(InterfazGrafica, wxFrame)
    EVT_BUTTON(wxID_ANY, InterfazGrafica::on_btn_iniciar)
wxEND_EVENT_TABLE()

InterfazGrafica::InterfazGrafica(IMotorSimulacion* motor)
    : wxFrame(nullptr, wxID_ANY, "Simulador de Gestor de Procesos", wxDefaultPosition, wxSize(1000, 800)),
      motor_(motor) {

    if (!motor_) {
        wxLogError("Motor de simulación no inicializado");
        return;
    }

    // Panel principal con sizers
    wxPanel* main_panel = new wxPanel(this);
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);

    // Panel de control
    wxPanel* control_panel = crear_panel_control();
    main_sizer->Add(control_panel, 0, wxEXPAND | wxALL, 5);

    // Panel de procesos (Notebook)
    wxPanel* procesos_panel = crear_panel_procesos();
    main_sizer->Add(procesos_panel, 1, wxEXPAND | wxALL, 5);

    // Panel de logs (Notebook)
    wxPanel* logs_panel = crear_panel_logs();
    main_sizer->Add(logs_panel, 1, wxEXPAND | wxALL, 5);

    main_panel->SetSizer(main_sizer);
    Centre();
}

InterfazGrafica::~InterfazGrafica() = default;

wxPanel* InterfazGrafica::crear_panel_control() {
    wxPanel* panel = new wxPanel(this);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Título
    wxStaticText* titulo = new wxStaticText(panel, wxID_ANY, "Panel de Control");
    wxFont font = titulo->GetFont();
    font.SetPointSize(14);
    font.MakeBold();
    titulo->SetFont(font);
    sizer->Add(titulo, 0, wxALL, 5);

    // Sub-panel de botones
    wxPanel* btn_panel = new wxPanel(panel);
    wxBoxSizer* btn_sizer = new wxBoxSizer(wxHORIZONTAL);

    btn_iniciar_ = new wxButton(btn_panel, wxID_ANY, "Iniciar");
    btn_paso_ = new wxButton(btn_panel, wxID_ANY, "▶ Paso Siguiente");
    btn_forzar_salida_ = new wxButton(btn_panel, wxID_ANY, "Forzar Salida");
    btn_reanudar_ = new wxButton(btn_panel, wxID_ANY, "Reanudar");
    btn_suspender_ = new wxButton(btn_panel, wxID_ANY, "Suspender");

    btn_sizer->Add(btn_iniciar_, 0, wxALL, 5);
    btn_sizer->Add(btn_paso_, 0, wxALL, 5);
    btn_sizer->Add(btn_forzar_salida_, 0, wxALL, 5);
    btn_sizer->Add(btn_reanudar_, 0, wxALL, 5);
    btn_sizer->Add(btn_suspender_, 0, wxALL, 5);

    btn_panel->SetSizer(btn_sizer);
    sizer->Add(btn_panel, 0, wxEXPAND | wxALL, 5);

    // Sub-panel de medidores (dashboard)
    wxPanel* gauge_panel = new wxPanel(panel);
    wxBoxSizer* gauge_sizer = new wxBoxSizer(wxHORIZONTAL);

    // Uso de CPU
    wxStaticBoxSizer* cpu_box = new wxStaticBoxSizer(wxVERTICAL, gauge_panel, "Uso de CPU (%)");
    gauge_cpu_ = new wxGauge(gauge_panel, wxID_ANY, 100, wxDefaultPosition, wxSize(150, 30));
    cpu_box->Add(gauge_cpu_, 0, wxEXPAND | wxALL, 5);

    // Uso de Memoria
    wxStaticBoxSizer* mem_box = new wxStaticBoxSizer(wxVERTICAL, gauge_panel, "Uso de Memoria (MB)");
    gauge_memoria_ = new wxGauge(gauge_panel, wxID_ANY, 4096, wxDefaultPosition, wxSize(150, 30));
    mem_box->Add(gauge_memoria_, 0, wxEXPAND | wxALL, 5);

    // Total de Procesos
    wxStaticBoxSizer* procs_box = new wxStaticBoxSizer(wxVERTICAL, gauge_panel, "Total Procesos");
    txt_procesos_ = new wxStaticText(gauge_panel, wxID_ANY, "0");
    wxFont big_font = txt_procesos_->GetFont();
    big_font.SetPointSize(16);
    big_font.MakeBold();
    txt_procesos_->SetFont(big_font);
    procs_box->Add(txt_procesos_, 0, wxALIGN_CENTER | wxALL, 5);

    gauge_sizer->Add(cpu_box, 1, wxEXPAND | wxALL, 5);
    gauge_sizer->Add(mem_box, 1, wxEXPAND | wxALL, 5);
    gauge_sizer->Add(procs_box, 1, wxEXPAND | wxALL, 5);

    gauge_panel->SetSizer(gauge_sizer);
    sizer->Add(gauge_panel, 0, wxEXPAND | wxALL, 5);

    panel->SetSizer(sizer);
    return panel;
}

wxPanel* InterfazGrafica::crear_panel_procesos() {
    wxPanel* panel = new wxPanel(this);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Título
    wxStaticText* titulo = new wxStaticText(panel, wxID_ANY, "Procesos y Colas");
    wxFont font = titulo->GetFont();
    font.SetPointSize(12);
    font.MakeBold();
    titulo->SetFont(font);
    sizer->Add(titulo, 0, wxALL, 5);

    // Notebook con pestañas
    notebook_procesos_ = new wxNotebook(panel, wxID_ANY);

    // Pestaña 1: Lista de Procesos
    wxPanel* lista_panel = new wxPanel(notebook_procesos_);
    wxBoxSizer* lista_sizer = new wxBoxSizer(wxVERTICAL);

    grid_procesos_ = new wxGrid(lista_panel, wxID_ANY);
    grid_procesos_->CreateGrid(0, 5);
    grid_procesos_->SetColLabelValue(0, "ID Proceso");
    grid_procesos_->SetColLabelValue(1, "Nombre");
    grid_procesos_->SetColLabelValue(2, "Estado");
    grid_procesos_->SetColLabelValue(3, "Prioridad");
    grid_procesos_->SetColLabelValue(4, "Memoria (MB)");

    for (int i = 0; i < 5; ++i) {
        grid_procesos_->SetColSize(i, 150);
    }

    // Botón para crear proceso
    wxButton* btn_crear = new wxButton(lista_panel, wxID_ANY, "Crear Nuevo Proceso");
    btn_crear->Bind(wxEVT_BUTTON, &InterfazGrafica::on_btn_crear_proceso, this);

    lista_sizer->Add(grid_procesos_, 1, wxEXPAND | wxALL, 5);
    lista_sizer->Add(btn_crear, 0, wxALL, 5);
    lista_panel->SetSizer(lista_sizer);
    notebook_procesos_->AddPage(lista_panel, "Lista de Procesos");

    // Pestaña 2: Cola Listos
    wxPanel* cola_listos = new wxPanel(notebook_procesos_);
    wxBoxSizer* cola_sizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* cola_txt = new wxStaticText(cola_listos, wxID_ANY, "Cola de Procesos LISTOS:\n(Próximos a ejecutar)");
    cola_sizer->Add(cola_txt, 1, wxEXPAND | wxALL, 5);
    cola_listos->SetSizer(cola_sizer);
    notebook_procesos_->AddPage(cola_listos, "Cola_listos");

    // Pestaña 3: Diagrama de Colas
    wxPanel* diagrama = new wxPanel(notebook_procesos_);
    wxBoxSizer* diag_sizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* diag_txt = new wxStaticText(diagrama, wxID_ANY, "Diagrama de Colas\n(Visualización de estados y transiciones)");
    diag_sizer->Add(diag_txt, 1, wxEXPAND | wxALL, 5);
    diagrama->SetSizer(diag_sizer);
    notebook_procesos_->AddPage(diagrama, "Diagrama de Colas");

    // Pestaña 4: Colas del Planificador
    wxPanel* colas_plan = new wxPanel(notebook_procesos_);
    wxBoxSizer* plan_sizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* plan_txt = new wxStaticText(colas_plan, wxID_ANY, "Colas del Planificador\n(FCFS, SJF, Round Robin, Prioridades)");
    plan_sizer->Add(plan_txt, 1, wxEXPAND | wxALL, 5);
    colas_plan->SetSizer(plan_sizer);
    notebook_procesos_->AddPage(colas_plan, "Colas del Planificador");

    sizer->Add(notebook_procesos_, 1, wxEXPAND | wxALL, 5);
    panel->SetSizer(sizer);
    return panel;
}

wxPanel* InterfazGrafica::crear_panel_logs() {
    wxPanel* panel = new wxPanel(this);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Título
    wxStaticText* titulo = new wxStaticText(panel, wxID_ANY, "Detalles y Logs");
    wxFont font = titulo->GetFont();
    font.SetPointSize(12);
    font.MakeBold();
    titulo->SetFont(font);
    sizer->Add(titulo, 0, wxALL, 5);

    // Notebook con pestañas
    notebook_logs_ = new wxNotebook(panel, wxID_ANY);

    // Pestaña 1: Logs de Eventos
    wxPanel* logs_panel = new wxPanel(notebook_logs_);
    wxBoxSizer* logs_sizer = new wxBoxSizer(wxVERTICAL);

    grid_logs_ = new wxGrid(logs_panel, wxID_ANY);
    grid_logs_->CreateGrid(0, 4);
    grid_logs_->SetColLabelValue(0, "TIEMPO");
    grid_logs_->SetColLabelValue(1, "TIPO");
    grid_logs_->SetColLabelValue(2, "MENSAJE");
    grid_logs_->SetColLabelValue(3, "CAUSA TERMINACION");

    for (int i = 0; i < 4; ++i) {
        grid_logs_->SetColSize(i, 200);
    }

    logs_sizer->Add(grid_logs_, 1, wxEXPAND | wxALL, 5);
    logs_panel->SetSizer(logs_sizer);
    notebook_logs_->AddPage(logs_panel, "Logs de Eventos");

    // Pestaña 2: Recursos Asignados
    wxPanel* recursos_panel = new wxPanel(notebook_logs_);
    wxBoxSizer* recursos_sizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* recursos_txt = new wxStaticText(recursos_panel, wxID_ANY, "Recursos Asignados a Procesos\n(E/S, Dispositivos, etc.)");
    recursos_sizer->Add(recursos_txt, 1, wxEXPAND | wxALL, 5);
    recursos_panel->SetSizer(recursos_sizer);
    notebook_logs_->AddPage(recursos_panel, "Recursos Asignados");

    // Pestaña 3: Gestor de Logs
    wxPanel* gestor_panel = new wxPanel(notebook_logs_);
    wxBoxSizer* gestor_sizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* gestor_txt = new wxStaticText(gestor_panel, wxID_ANY, "Gestor de Logs\n(Exportar, Limpiar, Filtrar)");
    gestor_sizer->Add(gestor_txt, 1, wxEXPAND | wxALL, 5);
    gestor_panel->SetSizer(gestor_sizer);
    notebook_logs_->AddPage(gestor_panel, "Gestor de Logs");

    // Pestaña 4: Algoritmos
    wxPanel* algo_panel = new wxPanel(notebook_logs_);
    wxBoxSizer* algo_sizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* algo_label = new wxStaticText(algo_panel, wxID_ANY, "Seleccionar Algoritmo de Planificación:");
    wxArrayString algoritmos;
    algoritmos.Add("FCFS");
    algoritmos.Add("SJF");
    algoritmos.Add("ROUND ROBIN");
    algoritmos.Add("PRIORIDADES");

    combo_algoritmos_ = new wxComboBox(algo_panel, wxID_ANY, "FCFS", wxDefaultPosition, wxSize(200, -1), algoritmos, wxCB_READONLY);

    algo_sizer->Add(algo_label, 0, wxALL, 5);
    algo_sizer->Add(combo_algoritmos_, 0, wxALL, 5);
    algo_panel->SetSizer(algo_sizer);
    notebook_logs_->AddPage(algo_panel, "Algoritmos");

    sizer->Add(notebook_logs_, 1, wxEXPAND | wxALL, 5);
    panel->SetSizer(sizer);
    return panel;
}

void InterfazGrafica::on_btn_iniciar(wxCommandEvent& event) {
    if (motor_) {
        wxString algo = combo_algoritmos_->GetStringSelection();
        motor_->iniciar(algo.ToStdString(), 10);
        wxLogMessage("Simulación iniciada con algoritmo: %s", algo);
    }
}

void InterfazGrafica::on_btn_paso(wxCommandEvent& event) {
    if (motor_) {
        motor_->ejecutarPasoSiguiente();
        actualizar_grid_procesos();
        actualizar_dashboard();
    }
}

void InterfazGrafica::on_btn_forzar_salida(wxCommandEvent& event) {
    // Obtener PID seleccionado del grid
    int row = grid_procesos_->GetGridCursorRow();
    if (row >= 0) {
        wxString pid_str = grid_procesos_->GetCellValue(row, 0);
        if (!pid_str.IsEmpty() && motor_) {
            uint32_t pid = wxAtoi(pid_str);
            motor_->forzarSalidaProceso(pid, "USUARIO");
            actualizar_grid_procesos();
        }
    } else {
        wxLogWarning("Selecciona un proceso primero");
    }
}

void InterfazGrafica::on_btn_reanudar(wxCommandEvent& event) {
    int row = grid_procesos_->GetGridCursorRow();
    if (row >= 0) {
        wxString pid_str = grid_procesos_->GetCellValue(row, 0);
        if (!pid_str.IsEmpty() && motor_) {
            uint32_t pid = wxAtoi(pid_str);
            motor_->cambiarEstadoSuspension(pid, false);
            actualizar_grid_procesos();
        }
    }
}

void InterfazGrafica::on_btn_suspender(wxCommandEvent& event) {
    int row = grid_procesos_->GetGridCursorRow();
    if (row >= 0) {
        wxString pid_str = grid_procesos_->GetCellValue(row, 0);
        if (!pid_str.IsEmpty() && motor_) {
            uint32_t pid = wxAtoi(pid_str);
            motor_->cambiarEstadoSuspension(pid, true);
            actualizar_grid_procesos();
        }
    }
}

void InterfazGrafica::on_btn_crear_proceso(wxCommandEvent& event) {
    mostrar_dialogo_crear_proceso();
}

void InterfazGrafica::actualizar_grid_procesos() {
    // Limpia el grid (implementación simplificada)
    while (grid_procesos_->GetNumberRows() > 0) {
        grid_procesos_->DeleteRows(0, 1);
    }
    // Aquí se añadirían filas con datos reales del motor
}

void InterfazGrafica::actualizar_dashboard() {
    if (motor_) {
        float cpu = motor_->obtenerUsoCPU();
        float mem = motor_->obtenerUsoMemoria();
        uint32_t procs = motor_->obtenerTotalProcesos();

        gauge_cpu_->SetValue((int)cpu);
        gauge_memoria_->SetValue((int)mem);
        txt_procesos_->SetLabel(wxString::Format("%u", procs));
    }
}

void InterfazGrafica::mostrar_dialogo_crear_proceso() {
    wxDialog dlg(this, wxID_ANY, "Crear Nuevo Proceso", wxDefaultPosition, wxSize(400, 300));
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Campo Nombre
    wxBoxSizer* nombre_sizer = new wxBoxSizer(wxHORIZONTAL);
    nombre_sizer->Add(new wxStaticText(&dlg, wxID_ANY, "Nombre del Proceso:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxTextCtrl* txt_nombre = new wxTextCtrl(&dlg, wxID_ANY, "proc_nuevo");
    nombre_sizer->Add(txt_nombre, 1, wxALL, 5);
    sizer->Add(nombre_sizer, 0, wxEXPAND | wxALL, 5);

    // Campo Ráfaga
    wxBoxSizer* rafaga_sizer = new wxBoxSizer(wxHORIZONTAL);
    rafaga_sizer->Add(new wxStaticText(&dlg, wxID_ANY, "Tiempo de Ráfaga:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxTextCtrl* txt_rafaga = new wxTextCtrl(&dlg, wxID_ANY, "5");
    rafaga_sizer->Add(txt_rafaga, 1, wxALL, 5);
    sizer->Add(rafaga_sizer, 0, wxEXPAND | wxALL, 5);

    // Campo Prioridad
    wxBoxSizer* prioridad_sizer = new wxBoxSizer(wxHORIZONTAL);
    prioridad_sizer->Add(new wxStaticText(&dlg, wxID_ANY, "Prioridad:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxTextCtrl* txt_prioridad = new wxTextCtrl(&dlg, wxID_ANY, "1");
    prioridad_sizer->Add(txt_prioridad, 1, wxALL, 5);
    sizer->Add(prioridad_sizer, 0, wxEXPAND | wxALL, 5);

    // Campo Memoria
    wxBoxSizer* memoria_sizer = new wxBoxSizer(wxHORIZONTAL);
    memoria_sizer->Add(new wxStaticText(&dlg, wxID_ANY, "Memoria Requerida (MB):"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    wxTextCtrl* txt_memoria = new wxTextCtrl(&dlg, wxID_ANY, "64");
    memoria_sizer->Add(txt_memoria, 1, wxALL, 5);
    sizer->Add(memoria_sizer, 0, wxEXPAND | wxALL, 5);

    // Botones OK y Cancelar
    wxBoxSizer* btn_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* btn_ok = new wxButton(&dlg, wxID_OK, "Crear");
    wxButton* btn_cancel = new wxButton(&dlg, wxID_CANCEL, "Cancelar");
    btn_sizer->Add(btn_ok, 0, wxALL, 5);
    btn_sizer->Add(btn_cancel, 0, wxALL, 5);
    sizer->Add(btn_sizer, 0, wxALIGN_CENTER | wxALL, 5);

    dlg.SetSizer(sizer);

    if (dlg.ShowModal() == wxID_OK && motor_) {
        std::string nombre = txt_nombre->GetValue().ToStdString();
        uint64_t rafaga = wxAtoi(txt_rafaga->GetValue());
        int prioridad = wxAtoi(txt_prioridad->GetValue());
        uint32_t memoria = wxAtoi(txt_memoria->GetValue());

        motor_->crearProceso(nombre, rafaga, prioridad, memoria);
        actualizar_grid_procesos();
        wxLogMessage("Proceso '%s' creado", nombre);
    }
}

} // namespace sim

