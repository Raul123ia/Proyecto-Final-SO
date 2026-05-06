cmake_minimum_required(VERSION 3.20)
project(prueba4)

# 1. Estándar de C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Configuración de rutas de Qt6
# Se recomienda usar '/' en lugar de '\' para evitar problemas de escape en Windows
set(CMAKE_PREFIX_PATH "C:/Qt/6.11.0/mingw_64")

# 3. Especificar ruta de Ninja (Solo si CLion no lo detecta automáticamente)
set(CMAKE_MAKE_PROGRAM "C:/Program Files/JetBrains/CLion 2026.1.1/bin/ninja/win/x64/ninja.exe")

# 4. Automatización de herramientas de Qt
set(CMAKE_AUTOUIC ON) # Procesa archivos .ui
set(CMAKE_AUTOMOC ON) # Procesa Meta-Object Compiler
set(CMAKE_AUTORCC ON) # Procesa recursos (.qrc)

# 5. Buscar módulos de Qt
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# 6. Definir archivos fuentes con sus nuevas rutas
# Es importante incluir los .h y .cpp para que AUTOMOC funcione correctamente
set(PROJECT_SOURCES
		src/main.cpp
		src/core/UI/InterfazGrafica.h
		src/UI/InterfazGrafica.cpp
)

# 7. Crear el ejecutable
add_executable(prueba4 ${PROJECT_SOURCES})

# 8. Configurar directorios de inclusión (IMPORTANTÍSIMO)
# Esto permite que hagas: #include "core/InterfazGrafica.h"
# También incluye la carpeta 'build' para los archivos generados por Qt (ui_*.h)
target_include_directories(prueba4 PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${CMAKE_CURRENT_BINARY_DIR}
)

# 9. Vincular librerías
target_link_libraries(prueba4 PRIVATE Qt6::Widgets)