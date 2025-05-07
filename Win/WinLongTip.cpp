#include <Windows.h>
#include <QPainter>

#include "App/Util.h"
#include "WinLongTip.h"
#include "WinLong.h"

WinLongTip::WinLongTip(QWidget *parent) : QMainWindow(parent)
{
    setAutoFillBackground(false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setFixedSize(60, 60);
    setFocusPolicy(Qt::NoFocus);
    auto hwnd = (HWND)winId();
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);

}

WinLongTip::~WinLongTip()
{}

void WinLongTip::mouseMove(const QPoint & nativePos)
{
    QPoint pos = QCursor::pos();
    move(pos.x() - 30, pos.y() - 30);
}

void WinLongTip::paintEvent(QPaintEvent * event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 0, 0, 120));
    auto r = rect();
    p.drawEllipse(r);
    p.setPen(QColor(255,255,255));
    p.setBrush(Qt::NoBrush);
    auto font = Util::getTextFont(16);
    p.setFont(*font);
    p.drawText(r, Qt::AlignCenter, "开始");
}

void WinLongTip::closeEvent(QCloseEvent* event)
{
    auto win = (WinLong*)parent();
    deleteLater();
    win->winLongTip = nullptr;
}
