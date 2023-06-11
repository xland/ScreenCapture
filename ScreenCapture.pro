QT += core gui widgets

CONFIG += c++20

win32-msvc*:QMAKE_CXXFLAGS += /utf-8
# QMAKE_LFLAGS += "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\""

SOURCES += \
    $$files($$PWD/src/*.cpp)

HEADERS += \
    $$files($$PWD/src/*.h)

FORMS += \
    $$files($$PWD/src/*.ui)

RESOURCES += \
    $$files($$PWD/src/*.qrc)

LIBS += -lKernel32\
    -lUser32 \
    -lGdi32 \
    -luser32 \
    -lShcore

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target