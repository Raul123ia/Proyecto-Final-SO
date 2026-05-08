# 🖥️ Simulador de Gestión de Procesos - Qt6/C++20

Interfaz gráfica moderna para un simulador de gestión de procesos del sistema operativo.

## 📁 Estructura del Proyecto

```
prueba4/
├── CMakeLists.txt              # Configuración CMake con Qt6
├── run.bat                      # Script para ejecutar la aplicación
├── README.md                    # Este archivo
├── src/
│   ├── main.cpp                # Punto de entrada
│   ├── ui/
│   │   ├── InterfazGrafica.h   # Clase principal de la interfaz
│   │   └── InterfazGrafica.cpp # Implementación de InterfazGrafica
│   ├── core/                   # (Preparado para lógica de procesos)
│   └── ipc/                    # (Preparado para comunicación entre procesos)
└── cmake-build-debug/          # Carpeta de compilación (auto-generada)
```

## ✨ Características

- **Interfaz moderna**: Tema dark con colores profesionales
- **Gestión de procesos**: Visualización de colas de planificación
- **Tablas interactivas**: QTableWidget para procesos y recursos
- **Monitoreo de memoria**: QProgressBar con uso total de 4096 MB
- **Historial de eventos**: Registro de acciones del sistema
- **Diálogos modales**: Para crear nuevos procesos
- **Datos mock**: Procesos de prueba precargados

## 🚀 Requisitos

- C++20 o superior
- Qt6 (instalado en `C:\Qt\6.11.0\mingw_64`)
- CMake 3.20+
- CLion 2026.1.1 (o compatible)
- MinGW con GCC/G++ 13+

## 🔧 Compilación

### Opción 1: Desde CLion
```
1. Abre el proyecto en CLion
2. Build → Rebuild Project
3. Run (Play button)
```

### Opción 2: Desde línea de comandos
```powershell
cd C:\Users\isaia\CLionProjects\prueba4
cmake -B cmake-build-debug -G Ninja `
  -DCMAKE_BUILD_TYPE=Debug `
  -DCMAKE_C_COMPILER="...mingw\bin\gcc.exe" `
  -DCMAKE_CXX_COMPILER="...mingw\bin\g++.exe" `
  -DCMAKE_MAKE_PROGRAM="...ninja\win\x64\ninja.exe"
cmake --build cmake-build-debug --config Debug
```

### Opción 3: Ejecutar directamente
```powershell
C:\Users\isaia\CLionProjects\prueba4\run.bat
```

## 📊 Componentes de la Interfaz

### Encabezado
- Título y descripción
- Barra de progreso de memoria (4096 MB)

### Botones principales
- **Siguiente Paso**: Ejecuta el siguiente proceso de la cola
- **Crear Proceso**: Abre diálogo para crear nuevo proceso

### Panel izquierdo
- **Colas de Planificación**: Dos QListWidget (Listos y Suspendidos)
- **Historial de Eventos**: QTextEdit con registro de acciones

### Panel derecho
- **Tabla de Procesos y Recursos**: QTableWidget con 6 columnas
  - PID
  - Nombre
  - Estado
  - Prioridad (0-9)
  - Ráfaga (en unidades de tiempo)
  - Memoria (MB)

## 🎮 Uso

1. **Ver procesos actuales**: Inmediatamente visible en la tabla
2. **Crear proceso**: Clic en "Crear Proceso" → Rellenar datos → OK
3. **Ejecutar paso**: Clic en "Siguiente Paso" para mover proceso de la cola
4. **Monitorar memoria**: La barra superior muestra uso total

## 📋 Datos Mock

Procesos precargados para pruebas:
- Navegador (PID 101, Listo, 640 MB)
- Editor (PID 102, Listo, 512 MB)
- Compilador (PID 103, Suspendido, 896 MB)
- Terminal (PID 104, Ejecución, 128 MB)
- ServicioBackup (PID 105, Suspendido, 704 MB)
- Monitor (PID 106, Listo, 256 MB)

## 🎨 Tema

- **Fondo**: #0F172A (azul muy oscuro)
- **Paneles**: #111827 (gris oscuro)
- **Texto**: #E5E7EB (gris claro)
- **Acento**: #2F80ED (azul profesional)
- **Bordes**: Sutil (1px, 8% opacidad)

## 📝 Próximos pasos

- Implementar motor de simulación en `src/core/`
- Agregar lógica de planificación (FIFO, RR, SJF, etc.)
- Implementar comunicación entre procesos en `src/ipc/`
- Conectar interfaz con motor lógico

## 📜 Licencia

Proyecto educativo - 2026

---

**¡Aplicación ejecutándose y funcionando correctamente!** ✅

