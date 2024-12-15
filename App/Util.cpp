#include <Windows.h>
#include <windowsx.h>
#include "Util.h"

QMouseEvent Util::createMouseEvent(const int& x, const int& y, const QEvent::Type& type, const Qt::MouseButton& btn)
{
    auto isCtrl = GetKeyState(VK_CONTROL) & 0x8000;
    auto isShift = GetKeyState(VK_SHIFT) & 0x8000;
    Qt::KeyboardModifiers mf = Qt::NoModifier;
    if (isCtrl) mf = Qt::ControlModifier;
    if (isShift) mf = mf | Qt::ShiftModifier;
    QPointF p(x, y);
    POINT pos;
    GetCursorPos(&pos);  //比ClientToScreen 效率要高
    QPointF g(pos.x, pos.y);  //todo 不应该用QPointF
    return QMouseEvent(QEvent::MouseButtonPress, p, g, btn, btn, mf);
}

QMouseEvent Util::createMouseEvent(const QEvent::Type& type, const Qt::MouseButton& btn)
{
    POINT pos; 
    GetCursorPos(&pos);
    return Util::createMouseEvent(pos.x, pos.y, type, btn);
}

QMouseEvent Util::createMouseEvent(const LPARAM& lParam, const QEvent::Type& type, const Qt::MouseButton& btn)
{
    auto x = GET_X_LPARAM(lParam);
    auto y = GET_Y_LPARAM(lParam); 
    return Util::createMouseEvent(x, y, type, btn);
}
