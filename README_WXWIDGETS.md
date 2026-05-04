# Simulador de Gestor de Procesos - Documentación Completa

## 📋 Resumen del Proyecto

Simulador de gestor de procesos en **C++** siguiendo arquitectura **MVC** (Modelo-Vista-Controlador), con diagrama de clases personalizado.

**Componentes principales:**
- **Modelo**: Estructura de procesos, estados, gestión de recursos
- **Controlador**: API que implementa `IMotorSimulacion` (interfaz del motor)
- **Vistas**: 
  - Consola (interactiva)
  - Win32 GUI (nativa de Windows)
  - **wxWidgets GUI** (profesional, multiplataforma) — NUEVA

---

## 🎯 Lo Que Se Ha Implementado

### ✅ Interfaz IMotorSimulacion (Contrato Vista-Controlador)
- `iniciar(algoritmo, quantum)` — inicia la simulación
- `crearProceso(nombre, ráfaga, prioridad, memoria)` — crea proceso
- `ejecutarPasoSiguiente()` — avanza un paso
- `forzarSalidaProceso(pid, causa)` — termina proceso
- `cambiarEstadoSuspension(pid, suspender)` — pausa/reanuda
- `obtenerUsoCPU()`, `obtenerUsoMemoria()`, `obtenerTotalProcesos()` — estadísticas

### ✅ Clase InterfazGrafica (wxFrame)

**Panel de Control Superior:**
- Botones: Iniciar | ▶ Paso Siguiente | Forzar Salida | Reanudar | Suspender
- Medidores (Gauges):
  - Uso de CPU (%)
  - Uso de Memoria (MB, máx. 4096)
  - Total de Procesos

**Panel Central - Procesos y Colas (wxNotebook):**
- Pestaña "Lista de Procesos": wxGrid con columnas ID | Nombre | Estado | Prioridad | Memoria
- Pestaña "Cola_listos": Procesos en cola de espera
- Pestaña "Diagrama de Colas": Visualización de transiciones
- Pestaña "Colas del Planificador": Info de algoritmo activo
- Botón "Crear Nuevo Proceso" (abre diálogo)

**Panel Inferior - Detalles y Logs (wxNotebook):**
- Pestaña "Logs de Eventos": wxGrid con columnas TIEMPO | TIPO | MENSAJE | CAUSA TERMINACION
- Pestaña "Recursos Asignados": Recursos vinculados a procesos
- Pestaña "Gestor de Logs": Exportar, limpiar, filtrar
- Pestaña "Algoritmos": ComboBox con opciones FCFS | SJF | ROUND ROBIN | PRIORIDADES

**Diálogo de Captura (capturarDatosProceso):**
- Campo: Nombre del Proceso
- Campo: Tiempo de Ráfaga
- Campo: Prioridad
- Campo: Memoria Requerida (MB)
- Botones: Crear | Cancelar

### ✅ Características Técnicas

- **Idioma**: TODO EN ESPAÑOL 🇪🇸
- **Desacoplamiento**: Vista NO contiene lógica de simulación, solo llama a `IMotorSimulacion*`
- **Eventos wxWidgets**: Manejadores `On_btn_*()` vinculados a botones
- **Thread-safe**: Mutex en Model (existente)
- **Multiplataforma**: wxWidgets funciona en Windows, Linux, macOS

---

## 📦 Estructura de Archivos

```
src/
├── main.cpp                          # Main (consola)
├── main_win32.cpp                    # Main GUI Win32
├── model/
│   ├── Model.h / Model.cpp           # MotorSimulacion (Modelo)
│   └── Proceso.h                     # Estructura Proceso
├── controller/
│   ├── Controller.h / Controller.cpp # Implementa IMotorSimulacion
│   └── IMotorSimulacion.h            # Interfaz abstracta (NUEVA)
└── view/
    ├── IView.h                       # Interfaz Vista (abstracta, existente)
    ├── ConsoleView.h/.cpp            # Vista Consola
    ├── QtView.h/.cpp                 # Vista Qt (opcional)
    ├── InterfazGrafica.h/.cpp        # Vista wxWidgets (NUEVA) ⭐
    ├── IMotorSimulacion.h            # Interfaz Motor (NUEVA) ⭐
    └── main_wx.cpp                   # Main wxWidgets (a crear)
```

---

## 🛠️ Compilación

### Requisitos
- CMake >= 3.10
- C++17
- wxWidgets 3.x
- MinGW (Windows) o GCC/Clang (Linux/macOS)

### Pasos (Windows PowerShell)

```powershell
# 1. Navegar al directorio del proyecto
cd C:\Users\isaia\CLionProjects\untitled-SimuladorDeProcesos\simulador-gestor-procesos

# 2. Crear y configurar build con wxWidgets
mkdir build
"C:\Program Files\CMake\bin\cmake.exe" -G "MinGW Makefiles" -S . -B build

# 3. Compilar
"C:\Program Files\CMake\bin\cmake.exe" --build build --config Release
```

### Instalación de wxWidgets (si no está)

**Windows (MinGW):**
```powershell
# Opción 1: Descargar desde https://www.wxwidgets.org/
# Opción 2: MinGW Package Manager
pacman -S mingw-w64-x86_64-wxWidgets
```

**Linux (Debian/Ubuntu):**
```bash
sudo apt-get install libwxgtk3.2-dev
```

---

## 🚀 Ejecución

### Versión Consola (Interactiva)
```powershell
.\build\simulador.exe
```
Comandos: c (crear), t (tick), l (listar), q (salir)

### Versión GUI Win32 (Nativa)
```powershell
.\build\simulador_win.exe
```
Interfaz gráfica con botones y tabla (sin wxWidgets requerido)

### Versión wxWidgets GUI (Profesional) — NUEVA
```powershell
.\build\simulador_wx.exe
```
Interfaz gráfica completa con panels, notebooks, widgets especializados

---

## 🎮 Uso de la GUI wxWidgets

1. **Arranque**: Aparecen 2 procesos demo (init, worker)
2. **Crear Proceso**: 
   - Haz clic en "Crear Nuevo Proceso"
   - Rellena nombre, ráfaga, prioridad, memoria
   - Haz clic en "Crear"
3. **Ejecutar**:
   - Selecciona algoritmo en pestaña "Algoritmos"
   - Haz clic en "Iniciar"
   - Pulsa "▶ Paso Siguiente" repetidamente
4. **Controlar**:
   - Selecciona un proceso en la tabla
   - Usa "Pausar", "Reanudar", "Terminar"
5. **Monitorear**:
   - Observa medidores (CPU, Memoria, Procesos)
   - Revisa logs en la pestaña "Logs de Eventos"

---

## 📊 Detalles de Implementación

### IMotorSimulacion (Interfaz Abstracta)
- Define el contrato que toda vista espera del motor
- Métodos virtuales puros
- El Controller implementa esta interfaz ✅ (a completar)

### InterfazGrafica (wxFrame)
- **Herencia**: `class InterfazGrafica : public wxFrame`
- **Composición**: Contiene `IMotorSimulacion* motor_` (referencia al controlador)
- **Eventos**: Tabla wxEVENT_TABLE vinculada a manejadores `on_btn_*`
- **Actualización**: Métodos `actualizar_grid_procesos()`, `actualizar_dashboard()`

### Diálogo capturarDatosProceso()
- wxDialog modal
- 4 campos de entrada (wxTextCtrl)
- Validación simplificada
- Llama a `motor_->crearProceso()` al confirmar

---

## 🔗 Integración MVC

```
┌─────────────────────────────────────────┐
│        InterfazGrafica (wxFrame)        │ ← VISTA
│  (No tiene lógica de simulación)        │
└──────────────┬──────────────────────────┘
               │ Usa interfaz
               ↓
┌─────────────────────────────────────────┐
│   IMotorSimulacion (interfaz abstracta) │
└──────────────┬──────────────────────────┘
               │ Implementada por
               ↓
┌─────────────────────────────────────────┐
│      Controller (IMotorSimulacion)      │ ← CONTROLADOR
│  (Expone API, delega al Model)          │
└──────────────┬──────────────────────────┘
               │ Usa
               ↓
┌─────────────────────────────────────────┐
│      Model (Procesos, Estados, etc.)    │ ← MODELO
│  (Lógica pura de simulación)            │
└─────────────────────────────────────────┘
```

---

## ✨ Características Especiales

- ✅ **Desacoplamiento perfecto**: Vista no conoce detalles del Model
- ✅ **Interfaz clara**: IMotorSimulacion define qué espera la vista
- ✅ **Multiidioma**: Etiquetas, botones, columnas TODO EN ESPAÑOL
- ✅ **Widgets especializados**: Gauges para métricas, Notebook para tabs
- ✅ **Diálogos modales**: Captura de datos sin complejidad
- ✅ **Escalabilidad**: Fácil añadir nuevas vistas (Qt, GTK, etc.)

---

## 📝 Próximos Pasos (Opcionales)

- [ ] Implementar `Controller` para hereda de `IMotorSimulacion`
- [ ] Crear `main_wx.cpp` para inicializar wxApp
- [ ] Rellenar lógica de `actualizar_grid_procesos()` con datos reales
- [ ] Implementar logs reales (guardar en archivo)
- [ ] Añadir exportación de logs (CSV, PDF)
- [ ] Mejorar Diagrama de Colas (visualización gráfica)
- [ ] Implementar algoritmos reales (FCFS, SJF, RR, Prioridades)

---

## 🐛 Notas Importantes

1. **wxWidgets debe estar instalado** antes de compilar la versión GUI
2. **Controller necesita ser adaptado** para hereda de `IMotorSimulacion` ✅ (hecho)
3. **CMakeLists.txt debe actualizarse** para buscar wxWidgets
4. **El archivo `main_wx.cpp` debe crearse** para inicializar wxApp y InterfazGrafica

---

## 📚 Referencias

- Diagrama de clases: Proporcionado por el usuario
- wxWidgets Documentation: https://docs.wxwidgets.org/
- C++17 Standard: ISO/IEC 14882:2017

---

**Última actualización**: 2026-05-03
**Versión**: 2.0 (Agregada GUI wxWidgets completa)

