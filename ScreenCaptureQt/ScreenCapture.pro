QT += core\
    gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ButtonDot.cpp \
    ColorSelector.cpp \
    Icon.cpp \
    MainWindowCanvas.cpp \
    MainWindowDragger.cpp \
    MainWindowInit.cpp \
    MainWindowMask.cpp \
    MainWindowMouse.cpp \
    MainWindowTool.cpp \
    MainWindowUndoRedo.cpp \
    PathModel.cpp \
    ScreenShoter.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    ButtonDot.h \
    ColorSelector.h \
    Icon.h \
    MainWindow.h \
    PathModel.h \
    ScreenShoter.h

FORMS += \
    ColorSelector.ui \
    MainWindow.ui

LIBS += -luser32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rc.qrc

DISTFILES += \
    ReadMe.md
