#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <QWindow>
#include <QFileDialog>
#include <QStandardPaths>
#include <QApplication>
#include <QClipboard>

#include "WinPin.h"
#include "WinFull.h"
#include "Canvas.h"
#include "Shape/ShapeBase.h"
#include "App/Util.h"
#include "App/Lang.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"


WinPin::WinPin(const QRect& r, const QImage& img, QWidget* parent) : WinBase(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setGeometry(r);
    initWindow();
    canvas = new Canvas(img, this);
}

WinPin::~WinPin()
{
   
}

void WinPin::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    canvas->paint(p);
}

void WinPin::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        posPress = event->globalPosition() - window()->pos();
    }
}

void WinPin::mouseMoveEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton) {
        auto p = event->globalPosition() - posPress;
        move(p.toPoint());
	}
}

void WinPin::mouseReleaseEvent(QMouseEvent* event)
{
}

void WinPin::initWindow()
{
    show();
    setAttribute(Qt::WA_DontCreateNativeAncestors);
    HWND hwnd = reinterpret_cast<HWND>(winId());
    MARGINS margins = { 1, 1, 1, 1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);
    int value = 2;
    DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &value, sizeof(value));
    DwmSetWindowAttribute(hwnd, DWMWA_ALLOW_NCPAINT, &value, sizeof(value));
}

