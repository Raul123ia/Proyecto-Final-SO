#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/grid.h>
#include <wx/gauge.h>
#include "IMotorSimulacion.h"

namespace sim {

class InterfazGrafica : public wxFrame {
public:
    explicit InterfazGrafica(IMotorSimulacion* motor);
    virtual ~InterfazGrafica();

private:
    IMotorSimulacion* motor_;

    // Paneles principales
    wxPanel* crear_panel_control();
    wxPanel* crear_panel_procesos();
    wxPanel* crear_panel_logs();

    // Widgets de control
    wxButton* btn_iniciar_;
    wxButton* btn_paso_;
    wxButton* btn_forzar_salida_;
    wxButton* btn_reanudar_;
    wxButton* btn_suspender_;

    // Medidores
    wxGauge* gauge_cpu_;
    wxGauge* gauge_memoria_;
    wxStaticText* txt_procesos_;

    // Notebooks
    wxNotebook* notebook_procesos_;
    wxNotebook* notebook_logs_;

    // Grids
    wxGrid* grid_procesos_;
    wxGrid* grid_logs_;

    // Combobox para algoritmos
    wxComboBox* combo_algoritmos_;

    // Manejadores de eventos
    void on_btn_iniciar(wxCommandEvent& event);
    void on_btn_paso(wxCommandEvent& event);
    void on_btn_forzar_salida(wxCommandEvent& event);
    void on_btn_reanudar(wxCommandEvent& event);
    void on_btn_suspender(wxCommandEvent& event);
    void on_btn_crear_proceso(wxCommandEvent& event);

    // Métodos auxiliares
    void actualizar_grid_procesos();
    void actualizar_dashboard();
    void mostrar_dialogo_crear_proceso();

    wxDECLARE_EVENT_TABLE();
};

} // namespace sim

