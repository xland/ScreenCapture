#include <Windows.h>
#include <QTimer>
#include "WinLong.h"
#include "CutMask.h"
#include "Canvas.h"
#include "App/Util.h"
#include "WinLongTip.h"
#include "WinLongViewer.h"

WinLong::WinLong(QWidget *parent) : WinBase(parent)
{
	x = GetSystemMetrics(SM_XVIRTUALSCREEN);
	y = GetSystemMetrics(SM_YVIRTUALSCREEN);
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	cutMask = new CutMask(this);
	imgBg = Util::printScreen(x, y, w, h);
	imgBg.setDevicePixelRatio(devicePixelRatio());
	initWindow();
}

WinLong::~WinLong()
{

}

void WinLong::paintEvent(QPaintEvent* event)
{
	QPainter p(this);
	p.drawImage(0,0, imgBg);
	cutMask->paint(p);
}

void WinLong::mousePressEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton) {
		if (state < State::mask) {
			cutMask->mousePress(event);
		}
		else if (state <= State::tool) {
			cutMask->mousePress(event);
		}
	}
	else {
		if (state < State::mask) {
			qApp->exit(2);
		}
	}
}

void WinLong::mouseMoveEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton) {
		if (state <= State::tool) {
			cutMask->mouseDrag(event);
		}
	}
	else {
		if (winLongTip) {
			winLongTip->mouseMove(event->pos());
		}
		if (state <= State::tool) {
			cutMask->mouseMove(event);
		}
	}
}

void WinLong::mouseReleaseEvent(QMouseEvent* event)
{
	if (state < State::capLong) {
		state = State::capLong;
		winLongTip = new WinLongTip(this);
		winLongTip->mouseMove(QCursor::pos());
		winLongTip->show();
	}
	else {
		winLongTip->close();
		imgBg.fill(QColor(0, 0, 0, 60));
		QPainter p(&imgBg);
		p.setCompositionMode(QPainter::CompositionMode_Clear); //橡皮擦模式
		p.setBrush(Qt::transparent);
		p.drawRect(cutMask->rectMask);
		p.end();
		repaint();
		auto dpr = devicePixelRatio();
		auto tl = cutMask->rectMask.topLeft();
		auto br = cutMask->rectMask.bottomRight();
		tl = Util::getScreenPos(mapToGlobal(tl));
		br = Util::getScreenPos(mapToGlobal(br));
		new WinLongViewer(tl.x()+2, tl.y() + 2, br.x() - tl.x()-2, br.y() - tl.y()-2,this);
	}
}

void WinLong::closeEvent(QCloseEvent* event)
{
	deleteLater();
}

void WinLong::initWindow()
{
	setAutoFillBackground(false);
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setGeometry(x, y, w, h);
#ifdef DEBUG
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
#else
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
#endif    
	show();
	auto hwnd = (HWND)winId();
	SetWindowPos(hwnd, nullptr, x, y, w, h, SWP_NOZORDER | SWP_SHOWWINDOW);
	setMouseTracking(true);
	setCursor(Qt::CrossCursor);
	setAttribute(Qt::WA_QuitOnClose, false);
}