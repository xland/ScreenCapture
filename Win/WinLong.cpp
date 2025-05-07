#include <Windows.h>
#include <QTimer>
#include "WinLong.h"
#include "CutMask.h"
#include "Canvas.h"
#include "App/Util.h"
#include "WinLongTip.h"

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

	//UINT scrollLines;
	//SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &scrollLines, 0);
	//int lines = delta * scrollLines / WHEEL_DELTA; // 计算滚动行数


	//SCROLLINFO si = { sizeof(SCROLLINFO) };
	//si.fMask = SIF_POS;
	//GetScrollInfo(hWnd, SB_VERT, &si);
	//int oldPos = si.nPos; // 记录滚动前位置

	//SetScrollInfo(hWnd, SB_VERT, &si, TRUE); // 更新滚动位置
	//GetScrollInfo(hWnd, SB_VERT, &si); // 获取新位置
	//int scrollDistance = si.nPos - oldPos; // 计算滚动距离（单位）


	//HDC hdc = GetDC(hWnd);
	//TEXTMETRIC tm;
	//GetTextMetrics(hdc, &tm);
	//int lineHeight = tm.tmHeight; // 每行高度（像素）
	//ReleaseDC(hWnd, hdc);
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
		setCursor(Qt::BlankCursor);
		winLongTip = new WinLongTip(this);
		winLongTip->mouseMove(QCursor::pos());
		winLongTip->show();
	}
	else {		
		hide();
		QTimer* timer = new QTimer(this);
		connect(timer, &QTimer::timeout, []() {
			INPUT input = { 0 };
			input.type = INPUT_MOUSE;
			input.mi.dwFlags = MOUSEEVENTF_WHEEL;
			input.mi.mouseData = -WHEEL_DELTA;
			SendInput(1, &input, sizeof(INPUT));
			qDebug() << "send";
		});
		timer->start(600);
		
	}
}

void WinLong::closeEvent(QCloseEvent* event)
{
	deleteLater();
	qApp->exit(1);
}

QImage WinLong::getTargetImg()
{
	return QImage();
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
}
