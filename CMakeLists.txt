cmake_minimum_required(VERSION 3.10)
# Nombre de tu proyecto
project(SimuladorGestorProcesos)

# Usar el estándar C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Le decimos al compilador dónde buscar los archivos .hpp
include_directories(src src/core src/ipc src/ui)

# Crear el ejecutable final vinculando el main y el gestor
add_executable(simulador src/main.cpp src/GestorComunicacion.cpp)