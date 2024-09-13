#include "CanvasWidget.h"
#include <qlayout.h>
#include <qpushbutton.h>
#include "WindowNative.h"


CanvasWidget::CanvasWidget(const int& x, const int& y, const int& w, const int& h,QWidget *parent)
	:x{ x }, y{ y }, w{ w }, h{h}, QWidget(parent)
{
	shotScreen();
	initPainter();
	setWindowFlags(Qt::FramelessWindowHint);
	QVBoxLayout* layout = new QVBoxLayout(this);
	QPushButton* button = new QPushButton("Click Me", this);
	connect(button, &QPushButton::clicked, this, &CanvasWidget::onButtonClicked);
	layout->addWidget(button);
	show();
}

CanvasWidget::~CanvasWidget()
{}

void CanvasWidget::onButtonClicked()
{
}

void CanvasWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.drawImage(rect(), *bg);
	painter.drawImage(rect(), *mask);
}


void CanvasWidget::shotScreen()
{
	HDC hScreen = GetDC(NULL); // 获取整个屏幕的设备上下文
	HDC hDC = CreateCompatibleDC(hScreen); // 创建内存设备上下文
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h); // 创建一个与屏幕设备上下文兼容的位图
	DeleteObject(SelectObject(hDC, hBitmap));  // 选择该位图到内存设备上下文中
	BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY); //将屏幕内容复制到位图
	long long dataSize = w * h * 4;
	bgPix.resize(dataSize);
	BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, (DWORD)dataSize, 0, 0, 0, 0 };
	GetDIBits(hDC, hBitmap, 0, h, &bgPix.front(), &info, DIB_RGB_COLORS); //获取位图的像素数据
	DeleteObject(hBitmap);
	DeleteDC(hDC);
	ReleaseDC(NULL, hScreen);
}

void CanvasWidget::initPainter()
{
	bg = new QImage(&bgPix.front(), w, h, QImage::Format_ARGB32);
	mask = new QImage(w, h, QImage::Format_ARGB4444_Premultiplied);
	mask->fill(0);
	maskPainter = new QPainter(mask);
	maskPainter->setRenderHint(QPainter::Antialiasing, true);
	QPen pen;
	pen.setColor(Qt::GlobalColor::red);
	pen.setWidth(26);
	maskPainter->setPen(pen);
	maskPainter->drawLine(0, 0, w, h);
}
