
#include "Cutter.h"
#include "qdebug.h"
#include <QCursor>

Cutter::Cutter(QObject *parent)
    : QObject{parent}
{
    getHoveredWindowRect();
}
void Cutter::getHoveredWindowRect()
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
