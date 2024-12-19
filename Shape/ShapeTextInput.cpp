#include <QPainter>
#include <Windows.h>
#include <QTimer>
#include <QWindow>
#include "../Win/WinBox.h"

#include "ShapeTextInput.h"
#include "ShapeText.h"

ShapeTextInput::ShapeTextInput(QWidget* parent) : QTextEdit(parent)
{
	setFrameStyle(QFrame::NoFrame);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);
	setLineWrapMode(QTextEdit::NoWrap);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setAttribute(Qt::WA_TransparentForMouseEvents, false);


	//auto timer = new QTimer(this);
	//QObject::connect(timer, &QTimer::timeout, [this]() {
	//	update();
	//});
	//timer->start(1000);
}

ShapeTextInput::~ShapeTextInput()
{}

ShapeTextInput* ShapeTextInput::create(ShapeText* parent)
{
	auto textEdit = new ShapeTextInput(); //不能把QObject设置为QWidget的parent
	textEdit->parent = parent;
	auto font = textEdit->font();
	font.setStyleStrategy(QFont::PreferAntialias);
	font.setPointSize(parent->fontSize);
	font.setWeight(parent->bold ? QFont::Bold : QFont::Normal);
	font.setItalic(parent->italic);
	textEdit->setFont(font);
	QString style{"QTextEdit{color:%1;margin:0px;padding:2px;background:transparent;}"};
	style = style.arg(parent->color.name());
	textEdit->setStyleSheet(style);
	
	auto doc = textEdit->document();
	connect(doc, &QTextDocument::contentsChanged, textEdit, &ShapeTextInput::adjustSize);
	connect(textEdit, &ShapeTextInput::focusOut, parent, &ShapeText::focusOut);
	//connect(textEdit, &ShapeTextInput::focusIn, parent, &ShapeText::focusIn);



	return textEdit;
}

void ShapeTextInput::moveTo(const QPoint& pos)
{
	move(pos);
	show();
	setText(""); //触发一次adjustSize
	setFocus();
	raise();
}

QRect ShapeTextInput::getNativeRect()
{
	RECT rect;
	GetWindowRect((HWND)winId(), &rect);
	POINT lt{ .x{rect.left},.y{rect.top} };
	POINT rb{ .x{rect.right},.y{rect.bottom} };
	auto win = (WinBox*)parent->parent();
	ScreenToClient(win->hwnd,&lt);
	ScreenToClient(win->hwnd,&rb);
	return QRect(QPoint(lt.x, lt.y+8), QPoint(rb.x, rb.y));
}

void ShapeTextInput::adjustSize()
{
	qDebug() << "adjustSize";
	auto doc = document();
	doc->adjustSize();
	auto size = doc->size().toSize();
	size += QSize(8, 4);
	setFixedSize(size);
}

void ShapeTextInput::focusOutEvent(QFocusEvent * event)
{
	qDebug() << "focusOutEvent";
	hide();
	emit focusOut();
}

//void ShapeTextInput::focusInEvent(QFocusEvent* event)
//{
//	QTextEdit::focusInEvent(event);
//	//emit focusIn();
//}

void ShapeTextInput::paintEvent(QPaintEvent* event)
{
	QTextEdit::paintEvent(event);
	if (isVisible()) {
		QPainter painter(viewport());
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setBrush(QColor(0, 0, 0, 8)); // 半透明背景
		painter.setPen(Qt::NoPen);
		painter.drawRect(rect());

		QPen pen;
		pen.setColor(parent->color);
		pen.setWidth(1);
		painter.setPen(pen);
		painter.setBrush(Qt::NoBrush);
		if (hasFocus() && showTextInputCursor) {
			auto cr = cursorRect();
			painter.drawLine(cr.topLeft(), cr.bottomLeft());
		}
		showTextInputCursor = !showTextInputCursor;		
		pen.setStyle(Qt::CustomDashLine);
		pen.setDashPattern({ 3,3 });
		painter.setPen(pen);
		painter.drawRect(1, 1, viewport()->width()-2, viewport()->height()-2);
	}
}
