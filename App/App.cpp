#include <QApplication>
#include "App.h"
#include "Lang.h"
#include "Win/WinFull.h"
#include "Win/WinPin.h"

namespace {
    std::unique_ptr<App> app; 
}
void App::init()
{
    QFont font("Microsoft YaHei", 9);
    qApp->setFont(font);
    Lang::init("ZhCn");
    new WinFull();
}
void App::dispose()
{
    app.reset();       
}
