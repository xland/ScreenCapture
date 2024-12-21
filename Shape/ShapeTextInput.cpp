#include <QPainter>
#include <Windows.h>
#include <QTimer>
#include <QWindow>
#include "../Win/WinBox.h"
#include "../Win/WinBoard.h"

#include "ShapeTextInput.h"
#include "ShapeText.h"

ShapeTextInput::ShapeTextInput(ShapeText* shapeText, QWidget* parent) : QTextEdit(parent),shapeText{shapeText}
{
	setAttribute(Qt::WA_TranslucentBackground, true);
	setFrameStyle(QFrame::NoFrame);
	setLineWrapMode(QTextEdit::NoWrap);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); 
	setFocusPolicy(Qt::StrongFocus);

	auto f = font();
	f.setStyleStrategy(QFont::PreferAntialias);
	f.setPointSize(shapeText->fontSize);
	f.setWeight(shapeText->bold ? QFont::Bold : QFont::Normal);
	f.setItalic(shapeText->italic);
	setFont(f);
	QString style{"color:%1;margin:0px;padding:0px;background:transparent;"};
	style = style.arg(shapeText->color.name());
	setStyleSheet(style);
}

ShapeTextInput::~ShapeTextInput()
{}

void ShapeTextInput::focusOutEvent(QFocusEvent* event)
{	
	event->ignore();
	QTimer::singleShot(20, this, [this, event]() {
		auto p = (ShapeTextContainer*)parent();
		if (p->isPress) return;
		QTextEdit::focusOutEvent(event);
		auto cursor = textCursor();
		cursor.clearSelection();
		setTextCursor(cursor);
	});




	//不能在这里paintOnBoard，因为鼠标按下准备拖动时，会触发focusOut，导致输入框消失
	//shapeText->paintOnBoard();
	//auto p = (ShapeTextContainer*)parent();
	//p->hide();
	//event->ignore();
}

//void ShapeTextInput::focusInEvent(QFocusEvent* event)
//{
//	QTextEdit::focusInEvent(event);
//}

//void ShapeTextInput::mouseMoveEvent(QMouseEvent* event)
//{
//	QTextEdit::mouseMoveEvent(event);
//	QGuiApplication::setOverrideCursor(Qt::IBeamCursor);
//}

void ShapeTextInput::paintEvent(QPaintEvent* event)
{
	QTextEdit::paintEvent(event);
	QPainter painter(viewport());
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setBrush(QColor(0, 0, 0, 1));
	painter.setPen(Qt::NoPen);
	painter.drawRect(rect());

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
