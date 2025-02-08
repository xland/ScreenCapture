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

namespace Win {
    // 注意：必须使用QOpenGLWindow，不能使用QOpenGLWideget
	Canvas::Canvas(Box* box) : QOpenGLWindow(), box{ box }
    {
        setFlags(Qt::FramelessWindowHint | Qt::Tool);		 
    }

    Canvas::~Canvas()
    {
    }

    void Canvas::fullWindow()
    {
        auto& rect = NativeRect::getDesktopRect();
        auto x = GetSystemMetrics(SM_XVIRTUALSCREEN);
        auto y = GetSystemMetrics(SM_YVIRTUALSCREEN);
        auto w = GetSystemMetrics(SM_CXVIRTUALSCREEN) + 1;  //必须大一点，不然第二次update时会出现黑屏闪烁
        auto h = GetSystemMetrics(SM_CYVIRTUALSCREEN) + 1;
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
        glClearColor(0.f, 0.f, 0.f, 0.f);  // 设置清空颜色（背景色）
    }

    void Canvas::paintGL()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        QPainter painter(this);
		painter.fillRect(QRect(0,0,width(),height()), Qt::transparent);
        painter.setRenderHint(QPainter::Antialiasing, true);

		if (box->state == State::start) {
			return;
		}

        painter.setBrush(QColor(0, 0, 0, 120));
        QPainterPath path;
        path.addRect(-1, -1, width() + 1, height() + 1);
        path.addRect(rectMask);
        painter.drawPath(path);
        QColor borderColor(22, 118, 255);
        painter.setPen(QPen(QBrush(borderColor), 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(rectMask);

    }

    void Canvas::mousePressEvent(QMouseEvent* event)
    {
        isPressed = true;
        posPress = event->pos();
    }

    void Canvas::mouseReleaseEvent(QMouseEvent* event)
    {
        isPressed = false;
    }

    void Canvas::mouseMoveEvent(QMouseEvent* event)
    {
        if (isPressed)
        {
            auto pos = event->pos();
            rectMask.setCoords(posPress.x(), posPress.y(), pos.x(), pos.y());
            rectMask = rectMask.normalized();
		    update(rectMask);
        }
        else {
			box->magnifier->mouseMove();
        }
    }

    void Canvas::closeEvent(QCloseEvent* event)
    {
        this->deleteLater();
    }

}