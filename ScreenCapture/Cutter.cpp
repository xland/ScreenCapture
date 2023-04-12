
#include "Cutter.h"
#include <QCursor>
#include <QApplication>
#include <QClipboard>

static Cutter* instance;

Cutter::Cutter(QObject *parent)
    : QObject{parent}
{
    createCutAreaByWindowRect();
}
Cutter::~Cutter()
{

}
void Cutter::Init(QObject *parent)
{
    instance = new Cutter(parent);
}
Cutter* Cutter::Get()
{
    return instance;
}
void Cutter::copyColor(bool isRgb)
{
    QClipboard* clipboard = QApplication::clipboard();
    if(isRgb){
        clipboard->setText(colorRgb);
    }else{
        clipboard->setText(colorHex);
    }
}
void Cutter::createCutAreaByWindowRect()
{
    auto position = QCursor::pos();
    auto shoter = ScreenShoter::Get();
    for (int i = 0; i < shoter->desktopWindowRects.count(); ++i) {
        if(shoter->desktopWindowRects[i].contains(position)){
            setCutAreaLeft(shoter->desktopWindowRects[i].left());
            setCutAreaTop(shoter->desktopWindowRects[i].top());
            setCutAreaRight(shoter->desktopWindowRects[i].right());
            setCutAreaBottom(shoter->desktopWindowRects[i].bottom());
//            qDebug() << "position" << shoter->desktopWindowRects[i].left()
//                     << shoter->desktopWindowRects[i].top()
//                     << shoter->desktopWindowRects[i].right()
//                     << shoter->desktopWindowRects[i].bottom();
            return;
        }
    }
}
void Cutter::setCutAreaLeft(int val)
{
    if(val == cutAreaLeft) return;
    cutAreaLeft = val;
    emit cutAreaLeftChanged(val);
}
void Cutter::setCutAreaTop(int val)
{
    if(val == cutAreaTop) return;
    cutAreaTop = val;
    emit cutAreaTopChanged(val);
}
void Cutter::setCutAreaRight(int val)
{
    if(val == cutAreaRight) return;
    cutAreaRight = val;
    emit cutAreaRightChanged(val);
}
void Cutter::setCutAreaBottom(int val)
{
    if(val == cutAreaBottom) return;
    cutAreaBottom = val;
    emit cutAreaBottomChanged(val);
}


void Cutter::setMouseX(int val)
{
    if(val == mouseX) return;
    mouseX = val;
    emit mouseXChanged(val);
}
void Cutter::setMouseY(int val)
{
    if(val == mouseY) return;
    mouseY = val;
    emit mouseYChanged(val);
}
void Cutter::setColorRgb(QString val)
{
    if(val == colorRgb) return;
    colorRgb = val;
    emit colorRgbChanged(val);
}
void Cutter::setColorHex(QString val)
{
    if(val == colorHex) return;
    colorHex = val;
    emit colorHexChanged(val);
}
