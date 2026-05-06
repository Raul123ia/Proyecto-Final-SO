cmake_minimum_required(VERSION 3.10)
# Nombre de tu proyecto
project(SimuladorGestorProcesos)

# Usar el estándar C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Le decimos al compilador dónde buscar los archivos .hpp
include_directories(src/core src/ipc src/ui)

# Crear el ejecutable final vinculando el main
# Conforme agreguen archivos .cpp (como scheduler.cpp o process.cpp), se listan aquí
add_executable(simulador
        src/core/GestorRecursos.cpp
        src/core/GestorRecursos/GestorRecursos.h
        src/GestorLogs/GestorLogs.cpp
        src/core/GestorLogs/GestorLogs.h
        src/CausaTerminacion/CausaTerminacion.cpp
        src/core/CausaTerminacion/CausaTerminacion.h
        src/GestorRecursos/GestorRecursos.cpp

)