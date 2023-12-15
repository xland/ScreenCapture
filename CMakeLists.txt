cmake_minimum_required(VERSION 3.18)
project(ScreenCapture)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
add_definitions(-DUNICODE -D_UNICODE -DNOMINMAX -D_SILENCE_ALL_CXX20_DEPRECATION_WARNINGS -DSK_ENABLE_SKSL)
link_directories(D:/sdk/skia/out)
include_directories(D:/sdk/skia)
set(SRC_LIST
    ./src/WindowBase.cpp
    ./src/WindowBase.h
    ./src/WindowMain.cpp
    ./src/WindowMain.h
    ./src/WindowPin.cpp
    ./src/WindowPin.h
    ./src/CutMask.cpp
    ./src/CutMask.h
    ./src/EventHandler.cpp
    ./src/EventHandler.h
    ./src/main.cpp
    ./src/Util.cpp
    ./src/Util.h
    ./src/AppFont.cpp
    ./src/AppFont.h
    ./src/ToolMain.cpp
    ./src/ToolMain.h
    ./src/ToolSub.cpp
    ./src/ToolSub.h
    ./src/State.h
    ./src/Shape/Pen.h
    ./src/Shape/ShapeBase.cpp
    ./src/Shape/ShapeBase.h
    ./src/Shape/Arrow.cpp
    ./src/Shape/Arrow.h
    ./src/Shape/Pen.cpp
    ./res/res.h
    ./res/res.rc
)
add_executable(ScreenCapture ${SRC_LIST})

if(WIN32)
    set_property(TARGET ScreenCapture PROPERTY VS_DPI_AWARE "PerMonitor")
    target_compile_definitions(ScreenCapture PUBLIC UNICODE _UNICODE)
    TARGET_LINK_LIBRARIES(ScreenCapture openGL32 Dwmapi sksg skottie skia winmm kernel32 user32 shell32 advapi32 imm32)
elseif(APPLE)
endif()