#include <qlayout.h>
#include <qpushbutton.h>


#include "CanvasWidget.h"
#include "WindowNative.h"

namespace {
	std::unique_ptr<CanvasWidget> canvasWidget;
}

CanvasWidget::CanvasWidget(QWidget *parent) : QWidget(parent)
{
	initImgs();
	setWindowFlags(Qt::FramelessWindowHint);
	auto winNative = WindowNative::Get();
	maskRect.setRect(-maskStroke, -maskStroke, winNative->w + maskStroke, winNative->h + maskStroke);	
	show();
}

CanvasWidget::~CanvasWidget()
{}

void CanvasWidget::Init()
{
	canvasWidget = std::make_unique<CanvasWidget>();

	auto hwnd = (HWND)canvasWidget->winId();
	auto winNative = WindowNative::Get();
	SetParent(hwnd, winNative->hwnd);
	SetWindowPos(hwnd, nullptr, 0, 0, winNative->w, winNative->h, SWP_NOZORDER | SWP_SHOWWINDOW);
	ShowWindow(winNative->hwnd, SW_SHOW);
}

CanvasWidget* CanvasWidget::Get()
{
	return canvasWidget.get();
}

void CanvasWidget::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton) {
		dragging = true;
		dragPosition = event->pos();
		if (state == State::start) {
			maskRect.setRect(dragPosition.x(), dragPosition.y(), 0, 0);
			state = State::mask;
		}
	}
	else if (event->button() == Qt::RightButton) {
		qApp->quit();
	}
	event->accept();
}

void CanvasWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (dragging && event->buttons() & Qt::LeftButton) {

		if (state == State::mask) {
			maskRect.setBottomRight(event->pos());
			update();
		}
	}
	event->accept();
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		if (state == State::mask) {
			state = State::tool;
			//auto btn = new QPushButton("allen", this);
			//btn->move(pos);
			//btn->show();
			ToolMain::Show();
			
		}
		dragging = false;
	}
	event->accept();
}

void CanvasWidget::onButtonClicked()
{
}

void CanvasWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.drawImage(rect(), *imgBg);
	painter.setRenderHint(QPainter::Antialiasing, true);
	paintMask(painter);	
	painter.end();
	QWidget::paintEvent(event);
}


void CanvasWidget::initImgs()
{
	auto winNative = WindowNative::Get();
	HDC hScreen = GetDC(NULL); 
	HDC hDC = CreateCompatibleDC(hScreen); 
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, winNative->w, winNative->h);
	DeleteObject(SelectObject(hDC, hBitmap)); 
	BOOL bRet = BitBlt(hDC, 0, 0, winNative->w, winNative->h, hScreen, winNative->x, winNative->y, SRCCOPY);
	long long dataSize = winNative->w * winNative->h * 4;
	std::vector<unsigned char> bgPix(dataSize, 0);
	//bgPix.resize(dataSize);
	BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)winNative->w, 0 - (long)winNative->h, 1, 32, BI_RGB, (DWORD)dataSize, 0, 0, 0, 0 };
	GetDIBits(hDC, hBitmap, 0, winNative->h, &bgPix.front(), &info, DIB_RGB_COLORS);
	DeleteObject(hBitmap);
	DeleteDC(hDC);
	ReleaseDC(NULL, hScreen);

	QImage bgTemp(&bgPix.front(), winNative->w, winNative->h, QImage::Format_ARGB32);
	//auto bg = bgTemp.convertToFormat(QImage::Format_RGB444); //QImage::Format_RGB444 让图像体积小一倍，但图像颜色会变得不一样
	//imgBg = std::make_unique<QImage>(std::move(bgTemp));
	imgBg = std::make_unique<QImage>(bgTemp.copy(0, 0, winNative->w, winNative->h)); 

}

void CanvasWidget::paintMask(QPainter& painter)
{
	QPainterPath temp0;
	temp0.addRect(-maskStroke, -maskStroke, width() + maskStroke, height() + maskStroke);
	QPainterPath temp1;
	temp1.addRect(maskRect);
	auto mask = temp0.subtracted(temp1);

	painter.setPen(QPen(QBrush(QColor(22, 119, 255)), maskStroke));
	painter.setBrush(QBrush(QColor(0, 0, 0, 120)));
	painter.drawPath(mask);
}
