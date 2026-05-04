#include <windows.h>
#include <commctrl.h>
#include <string>
#include <sstream>
#include <algorithm>
#include "model/Model.h"
#include "controller/Controller.h"

#ifndef SIMULADOR_CON_CMAKE
// Cuando CLion intenta compilar este archivo como unidad aislada,
// incluimos las implementaciones para evitar referencias indefinidas.
#include "model/Model.cpp"
#include "controller/Controller.cpp"
#endif

#pragma comment(lib, "comctl32.lib")

using namespace sim;

static Controller* g_controller = nullptr;
static HWND g_listbox = nullptr;
static HWND g_status = nullptr;
static HWND g_details = nullptr;
static int g_selected_pid = -1;

enum {
    ID_BTN_CREATE = 1,
    ID_BTN_TICK = 2,
    ID_BTN_EXIT = 3,
    ID_BTN_PAUSE = 4,
    ID_BTN_RESUME = 5,
    ID_BTN_KILL = 6,
    ID_LISTBOX = 100
};

std::wstring to_wstring_utf8(const std::string& s) {
    if (s.empty()) return {};
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

void update_listbox() {
    if (!g_controller || !g_listbox) return;
    const auto& procs = g_controller->model().getProcesses();

    SendMessageW(g_listbox, LB_RESETCONTENT, 0, 0);

    for (const auto& p : procs) {
        std::wstring estadoW = L"?";
        switch (p.estado) {
            case EstadoProceso::LISTO: estadoW = L"LISTO"; break;
            case EstadoProceso::EJECUTANDO: estadoW = L"EJECUTANDO"; break;
            case EstadoProceso::ESPERANDO: estadoW = L"ESPERANDO"; break;
            case EstadoProceso::TERMINADO: estadoW = L"TERMINADO"; break;
        }
        std::wstringstream ss;
        ss << L"[PID:" << p.pid << L"] " << to_wstring_utf8(p.nombre);
        ss << L" | " << estadoW << L" | Prioridad:" << p.prioridad;
        ss << L" | Ráfaga:" << p.rafaga_restante << L" | Memoria:" << p.memoria_asignada << L"MB";
        SendMessageW(g_listbox, LB_ADDSTRING, 0, (LPARAM)ss.str().c_str());
    }

    if (!g_status) return;
    uint32_t total = procs.size();
    uint32_t listos = 0, ejecutando = 0, terminados = 0;
    for (const auto& p : procs) {
        if (p.estado == EstadoProceso::LISTO) listos++;
        else if (p.estado == EstadoProceso::EJECUTANDO) ejecutando++;
        else if (p.estado == EstadoProceso::TERMINADO) terminados++;
    }
    std::wstringstream status;
    status << L"Total: " << total << L" | LISTOS: " << listos << L" | EJECUTANDO: " << ejecutando << L" | TERMINADOS: " << terminados;
    SetWindowTextW(g_status, status.str().c_str());
}

void update_details() {
    if (!g_controller || !g_details || g_selected_pid < 0) {
        SetWindowTextW(g_details, L"--- Selecciona un proceso en la lista ---");
        return;
    }
    const auto& procs = g_controller->model().getProcesses();

    for (const auto& p : procs) {
        if ((int)p.pid == g_selected_pid) {
            std::wstring estadoW = L"?";
            switch (p.estado) {
                case EstadoProceso::LISTO: estadoW = L"LISTO"; break;
                case EstadoProceso::EJECUTANDO: estadoW = L"EJECUTANDO"; break;
                case EstadoProceso::ESPERANDO: estadoW = L"ESPERANDO"; break;
                case EstadoProceso::TERMINADO: estadoW = L"TERMINADO"; break;
            }
            std::wstringstream ss;
            ss << L"=== DETALLES DEL PROCESO ===\r\n\r\n";
            ss << L"PID: " << p.pid << L"\r\n";
            ss << L"Nombre: " << to_wstring_utf8(p.nombre) << L"\r\n";
            ss << L"Estado: " << estadoW << L"\r\n";
            ss << L"Prioridad: " << p.prioridad << L"\r\n";
            ss << L"Ráfaga Restante: " << p.rafaga_restante << L" unidades\r\n";
            ss << L"Memoria: " << p.memoria_asignada << L" MB";
            SetWindowTextW(g_details, ss.str().c_str());
            return;
        }
    }
    SetWindowTextW(g_details, L"Proceso no encontrado");
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        // Botones superiores
        CreateWindowW(L"BUTTON", L"Crear Proceso", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            10, 10, 100, 30, hwnd, (HMENU)ID_BTN_CREATE, GetModuleHandle(NULL), NULL);
        CreateWindowW(L"BUTTON", L"▶ Avanzar", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            120, 10, 100, 30, hwnd, (HMENU)ID_BTN_TICK, GetModuleHandle(NULL), NULL);
        CreateWindowW(L"BUTTON", L"Pausar", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            230, 10, 80, 30, hwnd, (HMENU)ID_BTN_PAUSE, GetModuleHandle(NULL), NULL);
        CreateWindowW(L"BUTTON", L"Reanudar", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            320, 10, 80, 30, hwnd, (HMENU)ID_BTN_RESUME, GetModuleHandle(NULL), NULL);
        CreateWindowW(L"BUTTON", L"Terminar", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            410, 10, 80, 30, hwnd, (HMENU)ID_BTN_KILL, GetModuleHandle(NULL), NULL);
        CreateWindowW(L"BUTTON", L"Salir", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            500, 10, 80, 30, hwnd, (HMENU)ID_BTN_EXIT, GetModuleHandle(NULL), NULL);

        // Etiqueta "Procesos:"
        CreateWindowW(L"STATIC", L"Procesos en ejecución:", WS_VISIBLE | WS_CHILD,
            10, 50, 150, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);

        // ListBox para procesos
        g_listbox = CreateWindowW(L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL,
            10, 70, 570, 200, hwnd, (HMENU)ID_LISTBOX, GetModuleHandle(NULL), NULL);

        // Etiqueta "Detalles:"
        CreateWindowW(L"STATIC", L"Detalles del proceso seleccionado:", WS_VISIBLE | WS_CHILD,
            10, 280, 250, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);

        // Edit para detalles
        g_details = CreateWindowW(L"EDIT", L"--- Selecciona un proceso en la lista ---", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
            10, 300, 570, 80, hwnd, NULL, GetModuleHandle(NULL), NULL);

        // Barra de estado
        g_status = CreateWindowW(L"STATIC", L"Estado: Inicializando...", WS_VISIBLE | WS_CHILD | SS_SUNKEN,
            10, 390, 570, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);

        update_listbox();
        break;
    }
    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id == ID_LISTBOX && HIWORD(wParam) == LBN_SELCHANGE) {
            int index = SendMessageW(g_listbox, LB_GETCURSEL, 0, 0);
            if (index != LB_ERR) {
                const auto& procs = g_controller->model().getProcesses();
                if (index < (int)procs.size()) {
                    g_selected_pid = procs[index].pid;
                    update_details();
                }
            }
        } else if (id == ID_BTN_CREATE) {
            static int counter = 1;
            g_controller->crearProceso("proc_" + std::to_string(counter++), 5, 1, 64);
            update_listbox();
        } else if (id == ID_BTN_TICK) {
            g_controller->ejecutarPasoSiguiente();
            update_listbox();
        } else if (id == ID_BTN_PAUSE) {
            if (g_selected_pid >= 0) g_controller->cambiarEstadoSuspension(g_selected_pid, true);
            update_listbox();
        } else if (id == ID_BTN_RESUME) {
            if (g_selected_pid >= 0) g_controller->cambiarEstadoSuspension(g_selected_pid, false);
            update_listbox();
        } else if (id == ID_BTN_KILL) {
            if (g_selected_pid >= 0) g_controller->forzarSalidaProceso(g_selected_pid, "USUARIO");
            g_selected_pid = -1;
            update_listbox();
            update_details();
            PostQuitMessage(0);
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    Model model;
    Controller controller(model);
    g_controller = &controller;

    const wchar_t CLASS_NAME[] = L"SimuladorWindowClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(0, CLASS_NAME, L"Simulador de Gestor de Procesos v2.0",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 430, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    controller.crearProceso("init", 10, 1, 128);
    controller.crearProceso("worker", 7, 2, 64);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

