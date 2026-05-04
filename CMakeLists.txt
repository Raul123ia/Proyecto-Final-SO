cmake_minimum_required(VERSION 3.10)
# Nombre de tu proyecto
project(SimuladorGestorProcesos)

# Usar el estándar C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Le decimos al compilador dónde buscar los archivos .hpp
include_directories(src/core src/ipc src/ui)

set(SOURCES_COMMON
	src/model/Model.cpp
	src/controller/Controller.cpp
)

# Ejecutable GUI por defecto en Windows
if (WIN32)
	add_executable(simulador WIN32
		src/main_win32.cpp
		${SOURCES_COMMON}
	)
	target_compile_definitions(simulador PRIVATE SIMULADOR_CON_CMAKE)
	target_link_libraries(simulador PRIVATE comctl32)

	# Consola como target secundario
	add_executable(simulador_consola
		src/main.cpp
		${SOURCES_COMMON}
		src/view/ConsoleView.cpp
	)
	message(STATUS "Target principal: simulador (GUI Win32). Target secundario: simulador_consola")
else()
	add_executable(simulador
		src/main.cpp
		${SOURCES_COMMON}
		src/view/ConsoleView.cpp
	)
endif()

# Intentar habilitar GUI con Qt si está disponible
find_package(Qt6 COMPONENTS Widgets QUIET)
if (Qt6_FOUND)
	message(STATUS "Qt6 encontrado: habilitando objetivo GUI (simulador_qt)")
	set(CMAKE_AUTOMOC ON)
	set(SOURCES_QT
		src/main_qt.cpp
		src/view/QtView.cpp
	)
	add_executable(simulador_qt ${SOURCES_COMMON} ${SOURCES_QT})
	target_link_libraries(simulador_qt PRIVATE Qt6::Widgets)
endif()

# Añadir versión wxWidgets si está disponible
if (wxWidgets_FOUND)
	message(STATUS "wxWidgets encontrado: compilando simulador_wx")
	include(${wxWidgets_USE_FILE})

	add_executable(simulador_wx
		src/main_wx.cpp
		${SOURCES_COMMON}
		src/view/InterfazGrafica.cpp
	)

	target_include_directories(simulador_wx PRIVATE ${wxWidgets_INCLUDE_DIRS})
	target_link_libraries(simulador_wx PRIVATE ${wxWidgets_LIBRARIES})
else()
	message(STATUS "wxWidgets no encontrado: omitiendo simulador_wx")
endif()

