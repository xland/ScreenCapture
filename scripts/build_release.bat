@echo off

set QT_DIR=d:\NoQt\5.15.2\msvc2019
set SRC_DIR=%cd%
set BUILD_DIR=%cd%\build

if not exist %QT_DIR% exit
if not exist %SRC_DIR% exit
if not exist %BUILD_DIR% md %BUILD_DIR%

cd build

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x86

%QT_DIR%\bin\qmake.exe %SRC_DIR%\ScreenCapture.pro -spec win32-msvc  "CONFIG+=release"
if exist %BUILD_DIR%\release\ScreenCapture.exe del %BUILD_DIR%\release\ScreenCapture.exe
nmake Release
if not exist %BUILD_DIR%\release\Qt5Core.dll (
  %QT_DIR%\bin\windeployqt.exe %BUILD_DIR%\release\ScreenCapture.exe
)