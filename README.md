# Simulador de Gestor de Procesos

Implementación de un simulador de gestor de procesos en C++ siguiendo arquitectura MVC con modelo, controlador y múltiples vistas (consola, GUI Win32, GUI Qt opcional).

## Estructura del Proyecto

```
src/
├── main.cpp                 # Main para vista de consola
├── main_win32.cpp          # Main para GUI Win32
├── main_qt.cpp             # Main para GUI Qt (opcional, requiere Qt6)
├── model/
│   ├── Model.h/.cpp        # MotorSimulación, Proceso, EstadoProceso, TipoAlgoritmo
├── controller/
│   ├── Controller.h/.cpp   # API pública del simulador
└── view/
    ├── IView.h             # Interfaz de vista (abstracta)
    ├── ConsoleView.h/.cpp  # Implementación consola
    ├── QtView.h/.cpp       # Implementación Qt (opcional)
```

## Compilación

### Requisitos
- CMake >= 3.10
- Compilador G++ (MinGW en Windows)
- Qt6 (opcional, para GUI Qt)

### Pasos (Windows PowerShell)

```powershell
# 1. Navegar al repo
cd "C:\Users\isaia\CLionProjects\untitled-SimuladorDeProcesos\simulador-gestor-procesos"

# 2. Crear y configurar build
mkdir build
"C:\Program Files\CMake\bin\cmake.exe" -G "MinGW Makefiles" -S . -B build

# 3. Compilar
"C:\Program Files\CMake\bin\cmake.exe" --build build --config Debug
```

## Ejecución

### Versión Consola (Interactiva)
```powershell
.\build\simulador.exe
```

**Comandos disponibles:**
- `c` - Crear un nuevo proceso (valores por defecto: rafaga=5, prioridad=1, memoria=64MB)
- `t` - Ejecutar el siguiente paso (tick) de simulación
- `l` - Listar procesos y su estado actual
- `q` - Salir de la aplicación

**Ejemplo de sesión:**
```
> c
Proceso creado.
--- Estado del sistema ---
PID     Name    State   Priority        Burst   Mem
1       init    LISTO   1       10      128
2       worker  LISTO   2       7       64

> t
Tick ejecutado.
--- Estado del sistema ---
PID     Name    State   Priority        Burst   Mem
1       init    EJECUTANDO      1       9       128
2       worker  LISTO   2       7       64
```

### Versión GUI Win32 (Nativa)
```powershell
.\build\simulador_win.exe
```

**Interfaz:**
- Tabla con columnas: PID | Nombre | Estado | Prioridad | Ráfaga | Memoria
- Botones:
  - **Crear**: Añade un nuevo proceso
  - **Tick**: Avanza la simulación un paso
  - **Salir**: Cierra la aplicación

**Estados de los procesos:**
- `LISTO`: Aguardando ser ejecutado
- `EJECUTANDO`: En ejecución en la CPU
- `ESPERANDO`: Bloqueado (suspenso)
- `TERMINADO`: Finalizó su ejecución

### Versión GUI Qt (opcional)
Si tienes Qt6 instalado:
```powershell
.\build\simulador_qt.exe
```

## Clases Principales (según Diagrama UML)

### Proceso
- **Atributos:**
  - `pid` (uint32_t): Identificador único
  - `nombre` (string): Nombre del proceso
  - `estado` (EstadoProceso): LISTO, EJECUTANDO, ESPERANDO, TERMINADO
  - `prioridad` (int): Nivel de prioridad
  - `rafaga_restante` (uint64_t): Tiempo de CPU restante
  - `memoria_asignada` (uint32_t): Memoria en MB
  - `recursos_fisicos` (vector<string>): Recursos vinculados

- **Métodos:**
  - `obtenerPid()`: Retorna el PID
  - `actualizarEstado(nuevo)`: Cambia el estado
  - `vincularRecurso(nombre)`: Añade un recurso

### Model (MotorSimulación)
- `iniciar(algoritmo, quantum)`: Inicia la simulación con configuración
- `crearProceso(nombre, rafaga, prioridad, memoria)`: Crea un nuevo proceso
- `ejecutarPasoSiguiente()`: Avanza un paso de simulación
- `forzarSalidaProceso(pid, causa)`: Termina un proceso
- `cambiarEstadoSuspension(pid, suspender)`: Pausa/reanuda

### Controller
- Expone la API del Model a la vista
- Métodos: `iniciar()`, `crearProceso()`, `ejecutarPasoSiguiente()`, `forzarSalidaProceso()`, `cambiarEstadoSuspension()`

### Vistas
- **IView**: Interfaz abstracta
- **ConsoleView**: Implementación de línea de comandos
- **QtView**: Implementación con tabla Qt (opcional)

## Enums y Tipos

```cpp
enum class EstadoProceso { LISTO, EJECUTANDO, ESPERANDO, TERMINADO };
enum class TipoAlgoritmo { FCFS, SJF, ROUND_ROBIN, PRIORIDADES };
enum class CausaTerminacion { NORMAL, ERROR_, INTERBLOQUEO, USUARIO };
```

## Ejemplo de Flujo de Ejecución

1. Se crean 2 procesos iniciales: "init" (rafaga=10, prio=1) y "worker" (rafaga=7, prio=2).
2. Al pulsar "Tick":
   - El simulador elige un proceso en estado LISTO.
   - Lo cambia a EJECUTANDO.
   - Decrementa su ráfaga.
3. Se repite hasta que todos los procesos terminen (ráfaga = 0 → estado TERMINADO).

## Extensiones Futuras

- Implementar Planificador con políticas: FCFS, SJF, Round Robin, Prioridades.
- Añadir GestorRecursos, GestorLogs, GestorComunicación.
- Simular I/O y eventos de bloqueo.
- Mejorar GUI con paneles de detalles, estadísticas, historial.
- Tests unitarios.

## Notas

- El proyecto usa C++17 con mutex para thread-safety en el Model.
- Windows API (Win32) para GUI nativa sin dependencias externas (salvo Qt si eliges).
- CMake configurable para diferentes plataformas (Linux, macOS, Windows).

---

**Última actualización:** 2026-05-03

