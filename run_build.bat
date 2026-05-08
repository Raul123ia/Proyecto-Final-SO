@echo off
REM Script para configurar, compilar y ejecutar el proyecto usando CMake + Ninja y Qt (MinGW)
REM Ajusta la variable QT_DIR si tu instalación de Qt está en otra ruta.

set "QT_DIR=C:\Qt\6.11.0\mingw_64"
set "BUILD_DIR=cmake-build-debug"

REM Ir al directorio del script (raíz del proyecto) para evitar problemas con rutas con barra final
pushd "%~dp0"
echo Configurando CMake (Qt path: %QT_DIR%)... (working dir: %CD%)
cmake -S . -B "%BUILD_DIR%" -G "Ninja" -DCMAKE_PREFIX_PATH="%QT_DIR%"
if errorlevel 1 (
    echo Error durante la configuración de CMake.
    pause
    exit /b 1
)

echo Compilando el proyecto...
cmake --build "%BUILD_DIR%" --config Debug -- -j 8
if errorlevel 1 (
    echo Error durante la compilacion.
    pause
    exit /b 1
)

echo Ejecutando el binario...
start "" "%CD%\%BUILD_DIR%\prueba4.exe"
popd

exit /b 0


