#include <Windows.h>
#include <QTimer>
#include "WinLong.h"
#include "CutMask.h"
#include "WinPin.h"
#include "App/Util.h"
#include "WinLongTip.h"
#include "LongCapFuncs.h"
#include "Tool/ToolLong.h"

WinLong::WinLong(QWidget *parent) : WinBase(parent)
{
	x = GetSystemMetrics(SM_XVIRTUALSCREEN);
	y = GetSystemMetrics(SM_YVIRTUALSCREEN);
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	cutMask = new CutMask(this);

	imgBg = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
	imgBg.fill(QColor(0, 0, 0, 1));
	imgBg.setDevicePixelRatio(devicePixelRatio());
	initWindow();
}

WinLong::~WinLong()
{

}

void WinLong::pin()
{
	QScreen* screen = QGuiApplication::primaryScreen();
	auto dpr = screen->devicePixelRatio();
	imgResult.setDevicePixelRatio(dpr);
	int imageWidth = imgResult.width() / dpr;
	int imageHeight = imgResult.height() / dpr;
	QRect screenGeometry = screen->availableGeometry();
	int x = screenGeometry.x() + (screenGeometry.width() - imageWidth) / 2;
	int y = screenGeometry.y() + (screenGeometry.height() - imageHeight) / 2;
	new WinPin(QPoint(x, y), imgResult);
	close();
}

void WinLong::paintEvent(QPaintEvent* event)
{
	QPainter p(this);
	p.drawImage(0,0,imgBg);
	cutMask->paint(p);
	if (!imgResult.isNull()) {
		auto dpr = devicePixelRatio();
		qreal smallW{ (qreal)tools->width() * tools->devicePixelRatio()};
		qreal scaleVal = (qreal)imgResult.width() / smallW;
		qreal smallH = (qreal)imgResult.height() / scaleVal;
		auto smallImg = imgResult.scaled(smallW, smallH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		smallImg.setDevicePixelRatio(devicePixelRatio());
		auto pos = mapFromGlobal(tools->pos());
		p.drawImage(pos.x(),pos.y()- smallH / dpr-1, smallImg);
	}
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
		qApp->exit(2);
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
			if (cutMask->rectMask.contains(event->pos())) {
				if (!winLongTip->isVisible()) {
					winLongTip->show();
				}
				setCursor(Qt::BlankCursor);
				winLongTip->mouseMove(event->pos());
			}
			else {
				if (winLongTip->isVisible()) {
					winLongTip->hide();
				}
				setCursor(Qt::ArrowCursor);
			}
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
		if(winLongTip) winLongTip->close();
		setCursor(Qt::ArrowCursor);
		initTool();
		startCap();
	}
}

void WinLong::closeEvent(QCloseEvent* event)
{
	deleteLater();
}

QImage WinLong::getTargetImg()
{
	return imgResult;
}

void WinLong::initWindow()
{
	setAutoFillBackground(false);
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_TranslucentBackground, true);
	//setGeometry(x, y, w, h);
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

void WinLong::startCap()
{
	QPainter p(&imgBg);
	p.setCompositionMode(QPainter::CompositionMode_Clear);
	p.fillRect(cutMask->rectMask, Qt::transparent);
	p.end();
	repaint();//必须先重绘，不然图像像素对比不一样

	auto dpr = devicePixelRatio();
	auto tl = cutMask->rectMask.topLeft() * dpr;
	auto br = cutMask->rectMask.bottomRight() * dpr;
	areaX = tl.x()+2; 
	areaY = tl.y()+2; 
	areaW = br.x() - tl.x() - 2; 
	areaH = br.y() - tl.y() - 2;
	img1 = Util::printScreen(areaX, areaY, areaW, areaH);
	imgResult = img1;

	POINT pt;
	GetCursorPos(&pt);
	targetHwnd = WindowFromPoint(pt);

	stepTimer = new QTimer(this);
	connect(stepTimer, &QTimer::timeout, this, &WinLong::timerFunc);
	stepTimer->start(1200);
	SetCursorPos(pt.x + 1, pt.y + 1);
	update(); //必须再重绘一次，不然小图更新不及时
}

void WinLong::timerFunc()
{
	POINT pt;
	GetCursorPos(&pt);
	auto hwnd = WindowFromPoint(pt);
	if (hwnd != targetHwnd) return;
	INPUT input = { 0 };
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_WHEEL;
	input.mi.mouseData = -WHEEL_DELTA;
	SendInput(1, &input, sizeof(INPUT));
	//等待滚动完成后，再截图
	QTimer::singleShot(98, [this]() {  capStep(); });
}

void WinLong::capStep()
{
	auto img2 = Util::printScreen(areaX, areaY, areaW, areaH);
	static int startY{ -1 }, startX{ 999999 }, endY{ -1 }, endX{ -1 };
	if (firstCheck) {
		QList<QPoint> poss;
		for (size_t y = 0; y < img1.height(); y++)
		{
			for (size_t x = 0; x < img1.width(); x++)
			{
				if (img1.pixel(x, y) != img2.pixel(x, y)) {
					if (startY == -1) {
						startY = y;
					}
					poss.push_back(QPoint(x, y));
				}
			}
		}
		if (poss.isEmpty()) return;
		endY = poss.last().y();
		for (size_t i = 0; i < poss.size(); i++)
		{
			if (poss[i].x() < startX) startX = poss[i].x();
			if (poss[i].x() > endX) endX = poss[i].x();
		}
		firstCheck = false;
	}
	QImage img11 = img1.copy(startX, startY, endX, img1.height() - startY);
	QImage img22 = img2.copy(startX, startY, endX, 180);
	auto y = findMostSimilarRegionParallel(img11, img22);
	if (y == 0) {
		return;
	}
	auto paintStart = imgResult.height() - (img1.height() - y - startY);
	auto imgResultNew = QImage(imgResult.width(), paintStart + (img2.height() - startY), QImage::Format_ARGB32);
	auto img3 = img2.copy(0, startY, img2.width(), img2.height() - startY);

	QPainter p(&imgResultNew);
	p.drawImage(0, 0, imgResult);
	p.drawImage(0, paintStart, img3);
	p.end();
	img1 = img2;
	imgResult = imgResultNew;
	update();
}

void WinLong::initTool()
{
	if (tools) return;
	tools = new ToolLong(this);
	auto pos = mapToGlobal(cutMask->rectMask.bottomRight());
	QScreen* screen = QGuiApplication::screenAt(QPoint(pos.x()+tools->width(), pos.y()));
	if (screen) {
		tools->move(pos.x()+4, pos.y() - tools->height());
	}
	else {
		auto pos = mapToGlobal(cutMask->rectMask.bottomLeft());
		tools->move(pos.x() - tools->width()- 4, pos.y() - tools->height());
	}
	tools->show();
}
