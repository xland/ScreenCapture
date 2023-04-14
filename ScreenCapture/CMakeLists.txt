cmake_minimum_required(VERSION 3.16)

project(ScreenCapture VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick Widgets)
find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)

include_directories(${Qt6Widgets_INCLUDE_DIRS})
include_directories(${Qt6Core_INCLUDE_DIRS})

qt_standard_project_setup()

qt_add_executable(appScreenCapture
    main.cpp
    ScreenImageProvider.h ScreenImageProvider.cpp
    Cutter.h Cutter.cpp
    ScreenShoter.h ScreenShoter.cpp
    Res.qrc
)

qt_add_qml_module(appScreenCapture
    URI ScreenCapture
    VERSION 1.0
    QML_FILES Main.qml MouseTip.qml CutArea.qml DrawTool.qml
    ColorPicker.qml DrawRectTool.qml DrawBtn.qml
    ShapeRect.qml
    font.ttf
)

set_target_properties(appScreenCapture PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appScreenCapture
    PRIVATE Qt6::Quick Qt6::Widgets
)

install(TARGETS appScreenCapture
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
