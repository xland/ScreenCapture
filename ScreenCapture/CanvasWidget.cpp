#include "CanvasWidget.h"
#include <qlayout.h>
#include <qpushbutton.h>
#include "WindowNative.h"
#include "ToolMain.h"


CanvasWidget::CanvasWidget(const int& x, const int& y, const int& w, const int& h,QWidget *parent)
	:x{ x }, y{ y }, w{ w }, h{h}, QWidget(parent)
{
	shotScreen();
	initPainter();
	setWindowFlags(Qt::FramelessWindowHint);
	maskRect.setRect(-maskStroke, -maskStroke, w + maskStroke, h + maskStroke);
	//QVBoxLayout* layout = new QVBoxLayout(this);
	//QPushButton* button = new QPushButton("Click Me", this);
	//connect(button, &QPushButton::clicked, this, &CanvasWidget::onButtonClicked);
	//layout->addWidget(button);
	show();
}

CanvasWidget::~CanvasWidget()
{}

void CanvasWidget::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton) {
		dragging = true;
		dragPosition = event->pos();
		if (state == State::start) {
			maskRect.setRect(dragPosition.x(), dragPosition.y(), 0, 0);
			state = State::mask;
		}		
		event->accept();
	}
}

void CanvasWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (dragging && event->buttons() & Qt::LeftButton) {

		if (state == State::mask) {
			maskRect.setBottomRight(event->pos());
			update();
		}
		event->accept();
	}
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		if (state == State::mask) {
			state = State::tool;
			auto tm = new ToolMain(this);
			tm->move(event->pos());
			tm->show();
		}
		dragging = false;
		event->accept();
	}
}

void CanvasWidget::onButtonClicked()
{
}

void CanvasWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.drawImage(rect(), *imgBg);
	painter.drawImage(rect(), *imgBoard);
	painter.drawImage(rect(), *imgCanvas);
	painter.setRenderHint(QPainter::Antialiasing, true);
	paintMask(painter);	
	painter.end();
}


void CanvasWidget::shotScreen()
{
	HDC hScreen = GetDC(NULL); 
	HDC hDC = CreateCompatibleDC(hScreen); 
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h); 
	DeleteObject(SelectObject(hDC, hBitmap)); 
	BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY); 
	long long dataSize = w * h * 4;
	std::vector<unsigned char> bgPix(dataSize, 0);
	BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, (DWORD)dataSize, 0, 0, 0, 0 };
	GetDIBits(hDC, hBitmap, 0, h, &bgPix.front(), &info, DIB_RGB_COLORS); 
	DeleteObject(hBitmap);
	DeleteDC(hDC);
	ReleaseDC(NULL, hScreen);

	QImage bgTemp(&bgPix.front(), w, h, QImage::Format_ARGB32);
	auto bg = bgTemp.convertToFormat(QImage::Format_RGB444); //让图像体积小一倍
	imgBg = std::make_unique<QImage>(std::move(bg));

}

void CanvasWidget::initPainter()
{
	imgBoard = std::make_unique<QImage>(w, h, QImage::Format_ARGB4444_Premultiplied);
	imgBoard->fill(0);



	QPainter painterBoard(imgBoard.get());
	painterBoard.setRenderHint(QPainter::Antialiasing, true);
	QPen pen;
	pen.setColor(Qt::GlobalColor::red);
	pen.setWidth(26);
	painterBoard.setPen(pen);
	painterBoard.drawLine(0, 0, w, h);
	painterBoard.end();


	imgCanvas = std::make_unique<QImage>(w, h, QImage::Format_ARGB4444_Premultiplied);
	imgCanvas->fill(0);
}

void CanvasWidget::paintMask(QPainter& painter)
{
	QPainterPath temp0;
	temp0.addRect(-maskStroke, -maskStroke, w+ maskStroke, h+ maskStroke);
	QPainterPath temp1;
	temp1.addRect(maskRect);
	auto mask = temp0.subtracted(temp1);

	painter.setPen(QPen(QBrush(QColor(22, 119, 255)), maskStroke));
	painter.setBrush(QBrush(QColor(0, 0, 0, 120)));
	painter.drawPath(mask);
}
