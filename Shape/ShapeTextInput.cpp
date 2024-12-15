#include <QPainter>
#include <Windows.h>
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
	QString style{"color:%1;background:transparent;margin:1px;padding:2px;"};
	style = style.arg(parent->color.name());
	textEdit->textInputCursorColor = parent->color;
	textEdit->setStyleSheet(style);
	connect(textEdit->document(), &QTextDocument::contentsChanged, textEdit, &ShapeTextInput::adjustSize);
	connect(textEdit, &ShapeTextInput::focusOut, parent, &ShapeText::focusOut);
	connect(textEdit, &ShapeTextInput::focusIn, parent, &ShapeText::focusIn);
	return textEdit;
}

void ShapeTextInput::moveTo(const QPoint& pos)
{
	move(pos);
	show();
	setText(""); //触发一次adjustSize
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
	return QRect(QPoint(lt.x, lt.y), QPoint(rb.x, rb.y));
}

void ShapeTextInput::adjustSize()
{
	auto doc = document();
	doc->adjustSize();
	auto size = doc->size().toSize();
	if (doc->isEmpty()) {
		size += QSize(8, 6);
	}
	else {
		size += QSize(6, 2);
	}
	setFixedSize(size);
}

void ShapeTextInput::focusOutEvent(QFocusEvent * event)
{
	QTextEdit::focusOutEvent(event);
	emit focusOut();
}

void ShapeTextInput::focusInEvent(QFocusEvent* event)
{
	QTextEdit::focusInEvent(event);
	emit focusIn();
}

void ShapeTextInput::paintEvent(QPaintEvent* event)
{
	QTextEdit::paintEvent(event);
	if (isVisible()) {
		QPainter painter(viewport());
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setBrush(Qt::NoBrush);
		QPen pen;
		pen.setColor(textInputCursorColor);
		pen.setWidth(1);
		painter.setPen(pen);
		if (showTextInputCursor)
		{
			auto span = document()->toPlainText() == "" ? -1 : -4;
			auto cr = cursorRect().adjusted(0,1,0,span);
			painter.drawLine(cr.topLeft(),cr.bottomLeft());
		}
		pen.setStyle(Qt::CustomDashLine);
		pen.setDashPattern({ 3,3 });
		painter.setPen(pen);
		painter.drawRect(0, 0, viewport()->width(), viewport()->height());
	}
	showTextInputCursor = !showTextInputCursor;
}
