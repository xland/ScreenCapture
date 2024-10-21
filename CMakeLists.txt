cmake_minimum_required(VERSION 3.16)

project(ScreenCapture VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        BtnType.h
        State.h
        App.h
        App.cpp
        CanvasWidget.h
        CanvasWidget.cpp
        Tray.h
        Tray.cpp
        WindowNative.h
        WindowNative.cpp
        CutMask.h
        CutMask.cpp

        Tool/ToolBtn.h
        Tool/ToolMain.cpp
        Tool/ToolMain.h
        Tool/ToolSub.cpp
        Tool/ToolSub.h
        Tool/ColorCtrl.cpp
        Tool/ColorCtrl.h
        Tool/StrokeCtrl.cpp
        Tool/StrokeCtrl.h
        Tool/ToolBtn.cpp

        Shape/ShapeState.h
        Shape/ShapeBase.cpp
        Shape/ShapeBase.h
        Shape/ShapeRect.cpp
        Shape/ShapeRect.h
        Shape/ShapeEllipse.cpp
        Shape/ShapeEllipse.h

        QHotKey/qhotkey_p.h
        QHotKey/qhotkey_win.cpp
        QHotKey/qhotkey.cpp
        QHotKey/qhotkey.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ScreenCapture
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ScreenCapture APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ScreenCapture SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ScreenCapture
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(ScreenCapture PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ScreenCapture)
endif()
set_target_properties(ScreenCapture PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ScreenCapture
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ScreenCapture)
endif()
