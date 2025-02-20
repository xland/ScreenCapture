#include <QtWidgets/QApplication>
#include <QSurfaceFormat>

#include "App/App.h"

int main(int argc, char* argv[])
{
    QSurfaceFormat fmt;
	fmt.setVersion(3, 3);
    fmt.setDepthBufferSize(24);
    fmt.setSamples(16);
    fmt.setAlphaBufferSize(8);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setRenderableType(QSurfaceFormat::OpenGL);
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication a(argc, argv);
    if (!App::singleAppLock()) {
        return 0;
    }        
    App::init();
    auto resut = a.exec();
    App::dispose();
    return resut;
}
