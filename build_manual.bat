@echo off
REM Compila el proyecto manualmente usando moc + g++ (MinGW) — sólo si no quieres usar CMake/CLion
REM Ajusta QT_DIR si tu Qt está en otra ruta. Asegúrate de tener g++ en PATH.

setlocal enabledelayedexpansion
set "QT_DIR=C:\Qt\6.11.0\mingw_64"
set "MINGW=g++"
set "ROOT=%~dp0"
set "BUILD=%ROOT%build_manual"

echo Usando Qt en: %QT_DIR%
if not exist "%QT_DIR%\bin\moc.exe" (
  echo moc.exe no encontrado en %QT_DIR%\bin
  echo Ajusta QT_DIR en este archivo al directorio de tu instalacion de Qt.
  pause
  exit /b 1
)

if not exist "%BUILD%" mkdir "%BUILD%"
pushd "%BUILD%"

echo Generando moc para InterfazGrafica.h ...
"%QT_DIR%\bin\moc.exe" -I"%QT_DIR%\include" -I"%QT_DIR%\include\QtCore" "%ROOT%src\core\InterfazGrafica.h" -o moc_InterfazGrafica.cpp
if errorlevel 1 (
  echo moc ha fallado
  popd
  pause
  exit /b 1
)

echo Compilando InterfazGrafica.cpp ...
%MINGW% -std=gnu++20 -I"%QT_DIR%\include" -I"%QT_DIR%\include\QtWidgets" -I"%QT_DIR%\include\QtCore" -I"%QT_DIR%\include\QtGui" -I"%QT_DIR%\mkspecs\win32-g++" -g -c "%ROOT%src\ui\InterfazGrafica.cpp" -o InterfazGrafica.o
if errorlevel 1 (
  echo Compilacion de InterfazGrafica.cpp fallo
  popd
  pause
  exit /b 1
)

echo Compilando moc_InterfazGrafica.cpp ...
%MINGW% -std=gnu++20 -I"%QT_DIR%\include" -I"%QT_DIR%\include\QtCore" -I"%QT_DIR%\mkspecs\win32-g++" -g -c "moc_InterfazGrafica.cpp" -o moc_InterfazGrafica.o
if errorlevel 1 (
  echo Compilacion de moc falló
  popd
  pause
  exit /b 1
)

echo Compilando main.cpp ...
%MINGW% -std=gnu++20 -I"%QT_DIR%\include" -I"%QT_DIR%\include\QtWidgets" -I"%QT_DIR%\include\QtCore" -I"%QT_DIR%\mkspecs\win32-g++" -g -c "%ROOT%src\main.cpp" -o main.o
if errorlevel 1 (
  echo Compilacion de main.cpp fallo
  popd
  pause
  exit /b 1
)

echo Enlazando ejecutable (prueba_manual.exe) ...
%MINGW% main.o InterfazGrafica.o moc_InterfazGrafica.o -L"%QT_DIR%\lib" -lQt6Widgets -lQt6Gui -lQt6Core -lwinpthread -lgdi32 -static-libgcc -static-libstdc++ -o prueba_manual.exe
if errorlevel 1 (
  echo Enlace falló. Puede faltar alguna libreria o rutas incorrectas.
  echo Recomendado: usar CMake/CLion o ajustar las librerias en este script.
  popd
  pause
  exit /b 1
)

echo Ejecutable generado en %BUILD%\prueba_manual.exe
start "" "%BUILD%\prueba_manual.exe"
popd
exit /b 0

