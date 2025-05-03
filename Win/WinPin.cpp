#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <QWindow>
#include <QFileDialog>
#include <QStandardPaths>
#include <QApplication>
#include <QClipboard>
#include <QHBoxLayout>

#include "WinPin.h"
#include "WinFull.h"
#include "Canvas.h"
#include "Shape/ShapeBase.h"
#include "App/Util.h"
#include "App/Lang.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"
#include "WinPinBtns.h"


WinPin::WinPin(const QPoint& pos, QImage& img, QWidget* parent) : WinBase(parent)
{
    auto dpr = devicePixelRatio();
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window | Qt::WindowStaysOnTopHint);
    if (dpr != img.devicePixelRatio()) {
		img.setDevicePixelRatio(dpr);
    }
	setGeometry(pos.x(), pos.y(), img.width()/dpr, img.height()/dpr);
    initSize = size();
    initWindow();
    btns = new WinPinBtns(this);
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
    if (scaleNum != 1.0)
    {
        auto text = QString("Scale:%1").arg(QString::number(scaleNum, 'f', 2));
        auto font = Util::getTextFont(10);
        QFontMetrics fm(*font);
        p.setFont(*font);
        int w = fm.horizontalAdvance(text);
        QRect rect(0, 0, w + 14, 30);
        p.setBrush(QColor(0, 0, 0, 120));
        p.setPen(Qt::NoPen);
        p.drawRect(rect);
        p.setPen(QPen(QBrush(Qt::white), 1));
        p.setBrush(Qt::NoBrush);
        p.drawText(rect, Qt::AlignCenter, text);
    }
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

void WinPin::wheelEvent(QWheelEvent* event)
{
    if (toolMain) {
        toolMain->close();
        toolMain = nullptr;
        state = State::start;
        setCursor(Qt::SizeAllCursor);
    }
    int delta = event->angleDelta().y();    
    if (delta > 0) {        
        scaleNum += 0.06;
    }
    else if (delta < 0) {        
        scaleNum -= 0.06;
    }
    QSize newSize = initSize * scaleNum;
    resize(newSize);
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

void WinPin::resizeEvent(QResizeEvent* event)
{
    if (btns) {
        btns->move(width() - btns->width(), 0);
    }
}

void WinPin::leaveEvent(QEvent* event)
{
    if (btns->isVisible()) {
        btns->hide();
    }
}

void WinPin::enterEvent(QEnterEvent* event)
{
    if (!btns->isVisible()) {
        btns->show();
    }
}

