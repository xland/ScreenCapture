#include "ScreenCapture.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ScreenCapture w;
    w.show();
    return a.exec();
}
