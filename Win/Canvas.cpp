#include <QPainter>
#include <QTimer>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QGuiApplication>
#include <QScreen>
#include <QMouseEvent>
#include <QPainterPath>
#include <d2d1.h>
#include <dwmapi.h>
#include <versionhelpers.h>
#include <QWindow>
#include "Canvas.h"
#include "Box.h"
#include "Magnifier.h"
#include "../App/Util.h"
#include "../App/State.h"
#include "../App/NativeRect.h"


Canvas::Canvas(Box* box) : QOpenGLWindow(), box{ box }
{
    QSurfaceFormat format;
    format.setSamples(8);
    format.setDepthBufferSize(24);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(format);
    setFlags(Qt::FramelessWindowHint | Qt::Tool);
}

Canvas::~Canvas()
{
}

void Canvas::fullWindow()
{
    auto& rect = NativeRect::getDesktopRect();
    auto x{ rect.left() }, y{ rect.top() }, w{ rect.width() + 1 }, h{ rect.height() + 1 };  //必须大一点，不然第二次update时会出现黑屏闪烁
    //注意 必须先设置大小并显示窗口后，再使用原生API重置大小
    setMaximumSize(QSize(w, h));
    setMinimumSize(QSize(w, h));
    show();
    //注意 必须先重置大小，再设置缩放比例
    auto hwnd = (HWND)winId();
    SetWindowPos(hwnd, NULL, x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);  //HWND_TOPMOST  
	Util::alphaWindow(hwnd);
}

void Canvas::initializeGL()
{

    initializeOpenGLFunctions();

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glClearColor(0.f, 0.f, 0.f, 0.f);
}

void Canvas::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
	box->mask->paint(painter);
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    box->mask->mousePress(event);
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    box->mask->mouseRelease(event);
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
		box->mask->mouseDrag(event);
    }
    else {
        if (box->magnifier) box->magnifier->mouseMove(event);
        box->mask->mouseMove(event);
    }
}

void Canvas::closeEvent(QCloseEvent* event)
{
    this->deleteLater();
}