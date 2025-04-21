#include <QPainter>
#include <Windows.h>
#include <QTimer>
#include <QWindow>
#include "Win/WinBase.h"
#include "Win/Canvas.h"

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
	//不能在这里直接paintOnBoard，因为鼠标按下准备拖动时，会触发focusOut，导致输入框消失
	QTimer::singleShot(20, this, [this, event]() {
		auto p = (ShapeTextContainer*)parent();
		if (p->isPress) return;
		QTextEdit::focusOutEvent(event);
		auto cursor = textCursor();
		cursor.clearSelection();
		setTextCursor(cursor);
		if (document()->toPlainText().trimmed().isEmpty()) 
		{ 
			shapeText->deleteLater(); 
		}
		else {
			p->hide();
			shapeText->state = ShapeState::ready;
			auto win = (WinBase*)shapeText->parent();
			win->canvas->paintShapeOnBoard(shapeText);
		}
	});
}

void ShapeTextInput::mouseMoveEvent(QMouseEvent* event)
{
	QTextEdit::mouseMoveEvent(event);
	setCursor(Qt::IBeamCursor);
}

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
	pen.setWidthF(1.6);
	painter.setPen(pen);
	painter.setBrush(Qt::NoBrush);
	if (hasFocus() && showTextInputCursor) {
		auto cr = cursorRect();
		painter.drawLine(cr.topLeft(), cr.bottomLeft());
	}
	showTextInputCursor = !showTextInputCursor;
}
