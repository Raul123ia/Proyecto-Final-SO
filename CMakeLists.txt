cmake_minimum_required(VERSION 3.16)
project(SimuladorGestorProcesos)

# 1. Estándar de C++ (Usaremos 17 que es el estándar del curso)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Automatización de herramientas de Qt (Necesario para la UI)
# Esto permite que CMake maneje los archivos .ui y las macros de Qt automáticamente
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 3. Buscar módulos de Qt (De forma silenciosa para que no te dé error a ti)
find_package(Qt6 COMPONENTS Widgets QUIET)

# 4. Homologación de Fuentes (Tu estrategia de Integrador)
# Buscamos automáticamente todos los archivos en src para no listarlos uno por uno
file(GLOB_RECURSE SOURCES 
    "src/*.cpp" 
    "src/*.h"
    "src/*.ui"
)

# 5. Crear el ejecutable
add_executable(SimuladorGestorProcesos ${SOURCES})

# 6. Configurar directorios de inclusión
# Esto permite hacer #include "core/planificador/archivo.h" sin rutas largas
target_include_directories(SimuladorGestorProcesos PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)

# 7. Vincular librerías de Qt solo si se encuentran en el sistema
if(Qt6_FOUND)
    target_link_libraries(SimuladorGestorProcesos PRIVATE Qt6::Widgets)
    message(STATUS "Configuración de Qt6 cargada correctamente.")
else()
    message(WARNING "Qt6 no encontrado. Si no vas a compilar la UI, puedes ignorar esto.")
endif()