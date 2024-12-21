#include <QPainter>
#include <Windows.h>
#include <QVBoxLayout>
#include <QWindow>

#include "ShapeTextInput.h"
#include "../Win/WinBox.h"
#include "../Win/WinBoard.h"
#include "ShapeTextContainer.h"
#include "ShapeText.h"

ShapeTextContainer::ShapeTextContainer(ShapeText* shapeText, QWidget* parent) : QWidget(parent),shapeText{shapeText}
{
	setAttribute(Qt::WA_TranslucentBackground, true);
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
	setAttribute(Qt::WA_InputMethodEnabled, true);
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_OpaquePaintEvent, false);
	setAttribute(Qt::WA_NoSystemBackground);
	setFocusPolicy(Qt::StrongFocus);
	setMouseTracking(true);
	setAttribute(Qt::WA_Hover);

	shapeTextInput = new ShapeTextInput(shapeText,this);
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(3,3,3,3);
	layout->setSpacing(0);
	layout->addWidget(shapeTextInput);
	setLayout(layout);

	connect(shapeTextInput->document(), &QTextDocument::contentsChanged, this, &ShapeTextContainer::adjustSize);
	shapeTextInput->setText("");  //触发一次adjustSize
	shapeTextInput->setFocus();
	move(QCursor::pos() + QPoint(-10, -10));
	show();
	raise();
	activateWindow();
}

ShapeTextContainer::~ShapeTextContainer()
{
}

void ShapeTextContainer::setNativeRect()
{
	RECT r;
	GetWindowRect((HWND)winId(), &r);
	POINT lt{ .x{r.left},.y{r.top} };
	POINT rb{ .x{r.right},.y{r.bottom} };
	auto win = (WinBox*)shapeText->parent();
	ScreenToClient(win->hwnd,&lt);
	ScreenToClient(win->hwnd,&rb);
	ctrlRect = QRect(QPoint(lt.x, lt.y+8), QPoint(rb.x, rb.y));
}

void ShapeTextContainer::adjustSize()
{
	auto doc = shapeTextInput->document();
	doc->adjustSize();
	setFixedSize(doc->size().toSize()+QSize(6,6));
	setNativeRect();
}

void ShapeTextContainer::paintEvent(QPaintEvent* event)
{
	QWidget::paintEvent(event);
	if (painting) return;
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setBrush(QColor(0, 0, 0,1));
	painter.setPen(Qt::NoPen);
	painter.drawRect(rect());
	QPen pen;
	pen.setColor(shapeText->color);
	pen.setStyle(Qt::CustomDashLine);
	pen.setDashPattern({ 3,3 });
	pen.setWidth(1);
	painter.setPen(pen);
	painter.setBrush(Qt::NoBrush);
	QRectF rect(1, 1, width() - 2, height() - 2);
	painter.drawRect(rect);
}

void ShapeTextContainer::mouseMoveEvent(QMouseEvent* event)
{
	QGuiApplication::setOverrideCursor(Qt::SizeAllCursor);
	if (isPress) {
		auto pos = QCursor::pos();
		auto span = pos - pressPos;
		auto p = mapToGlobal(QPoint(0, 0)) + span;
		move(p.x(), p.y());
		pressPos = pos;
	}
}

void ShapeTextContainer::mousePressEvent(QMouseEvent* event)
{
	pressPos = QCursor::pos();
	isPress = true;
}

void ShapeTextContainer::mouseReleaseEvent(QMouseEvent* event)
{
	setNativeRect();
	isPress = false;
}
