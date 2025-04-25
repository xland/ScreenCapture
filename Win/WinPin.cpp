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


WinPin::WinPin(const QPoint& pos, QImage& img, QWidget* parent) : WinBase(parent)
{
    auto dpr = devicePixelRatio();
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    if (dpr != img.devicePixelRatio()) {
		img.setDevicePixelRatio(dpr);
    }
	setGeometry(pos.x(), pos.y(), img.width()/dpr, img.height()/dpr);
    initWindow();
    canvas = new Canvas(img, this);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &WinPin::showContextMenu);
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
        if (state >= State::rect) {
            canvas->mousePress(event);
        }
        else {
            posPress = event->globalPosition() - window()->pos();
        }
    }
}

void WinPin::mouseMoveEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton) {
        if (state >= State::rect) {
            canvas->mouseDrag(event);
		}
		else {
            auto p = event->globalPosition() - posPress;
            move(p.toPoint());
		}
    }
    else
    {
        if (state >= State::rect) {
            canvas->mouseMove(event);
        }
    }
}

void WinPin::mouseReleaseEvent(QMouseEvent* event)
{
    if (state >= State::rect) {
        canvas->mouseRelease(event);
    }
}

void WinPin::initWindow()
{
    setAttribute(Qt::WA_DontCreateNativeAncestors);
    setMouseTracking(true);
    setCursor(Qt::SizeAllCursor);
    show();
    HWND hwnd = reinterpret_cast<HWND>(winId());
    MARGINS margins = { 1, 1, 1, 1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);
    int value = 2;
    DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &value, sizeof(value));
    DwmSetWindowAttribute(hwnd, DWMWA_ALLOW_NCPAINT, &value, sizeof(value));
}

void WinPin::showContextMenu(const QPoint& pos)
{
    QMenu contextMenu(this);
    QAction* menuAction = contextMenu.addAction(Lang::get("toolBar"));
    menuAction->setCheckable(true);
	if (toolMain) {
        menuAction->setChecked(true);
	}
    QAction* closeAction = contextMenu.addAction(Lang::get("quit"));
    connect(menuAction, &QAction::triggered, [this]() {
		if (toolMain) {
			toolMain->close();
			toolMain = nullptr;
            this->setCursor(Qt::SizeAllCursor);
        }
        else {
            toolMain = new ToolMain(this);
            auto pos = this->pos();
            toolMain->move(pos.x(), pos.y() + this->height() + 4);
            toolMain->show();
        }
        });
    connect(closeAction, &QAction::triggered, [this](){
        close();
        });
    contextMenu.exec(mapToGlobal(pos));
}

QImage WinPin::getTargetImg()
{
    QPainter p(&canvas->imgBg);
    p.drawImage(QPoint(0, 0), canvas->imgBoard);
    return canvas->imgBg;
}

void WinPin::moveEvent(QMoveEvent* event)
{
	if (!toolMain) return;
    auto pos = this->pos();
    toolMain->move(pos.x(), pos.y() + height() + 4);
}

void WinPin::closeEvent(QCloseEvent* event)
{
    deleteLater();
}

