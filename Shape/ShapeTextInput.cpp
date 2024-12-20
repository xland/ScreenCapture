#include <QPainter>
#include <Windows.h>
#include <QTimer>
#include <QWindow>
#include "../Win/WinBox.h"

#include "ShapeTextInput.h"
#include "ShapeText.h"

ShapeTextInput::ShapeTextInput(ShapeText* shapeText, QWidget* parent) : QTextEdit(parent),shapeText{shapeText}
{
	setAttribute(Qt::WA_TranslucentBackground, true);
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);

	setFrameStyle(QFrame::NoFrame);
	setLineWrapMode(QTextEdit::NoWrap);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setAttribute(Qt::WA_TransparentForMouseEvents, false);

	auto f = font();
	f.setStyleStrategy(QFont::PreferAntialias);
	f.setPointSize(shapeText->fontSize);
	f.setWeight(shapeText->bold ? QFont::Bold : QFont::Normal);
	f.setItalic(shapeText->italic);
	setFont(f);
	QString style{ "QTextEdit{color:%1;margin:0px;padding:0px;background:transparent;}" };
	style = style.arg(shapeText->color.name());
	setStyleSheet(style);
	setText("");  //触发一次adjustSize
}

ShapeTextInput::~ShapeTextInput()
{}

void ShapeTextInput::focusOutEvent(QFocusEvent * event)
{
	qDebug() << "focusOutEvent";
	if (creating) {
		return;
	}
	shapeText->focusOut();
}

void ShapeTextInput::focusInEvent(QFocusEvent* event)
{
	QTextEdit::focusInEvent(event);
	shapeText->focusIn();
}

void ShapeTextInput::paintEvent(QPaintEvent* event)
{
	QTextEdit::paintEvent(event);
	QPainter painter(viewport());
	painter.setRenderHint(QPainter::Antialiasing, true);
	QPen pen;
	pen.setColor(shapeText->color);
	pen.setWidth(1);
	painter.setPen(pen);
	painter.setBrush(Qt::NoBrush);
	if (hasFocus() && showTextInputCursor) {
		auto cr = cursorRect();
		painter.drawLine(cr.topLeft(), cr.bottomLeft());
	}
	if (showTextInputCursor && creating) {
		//输入光标第一次闪烁之后，才算成功创建
		//不然，第一次创建输入框没问题，第二次创建输入框时，会导致第二次创建的输入框马上进入focusOut
		creating = false;
	}
	showTextInputCursor = !showTextInputCursor;
}
