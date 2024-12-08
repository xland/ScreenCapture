#include <qpainter.h>
#include <QTransform>
#include <QTextDocument>
#include <numbers>
#include <QTextCharFormat>
#include <QTextOption>

#include "ShapeText.h"
#include "../Win/WinBox.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinCanvas.h"


ShapeText::ShapeText(QObject* parent) : ShapeBase(parent)
{
    auto win = (WinBox*)parent;
    //state = ShapeState::ready;
    color = win->toolSub->getColor();
    fontSize = win->toolSub->getStrokeWidth();
    bold = win->toolSub->getSelectState("bold");
    italic = win->toolSub->getSelectState("italic");
}

ShapeText::~ShapeText()
{
    delete textEdit;
}

void ShapeText::adjustSize()
{
    textEdit->document()->adjustSize();
    auto size = textEdit->document()->size().toSize();
    if (textEdit->document()->isEmpty()) {
        size += QSize(8, 6); 
    }
    else {
        size += QSize(6, 2);
    }
    textEdit->setFixedSize(size);
    auto win = (WinBox*)parent();
    win->refreshBoard();  //为什么要在board上画框
}

void ShapeText::createTextEdit()
{
    textEdit = new ShapeTextInput();
    QFont font = textEdit->font();
    font.setPointSize(fontSize);
    font.setWeight(bold ? QFont::Bold : QFont::Normal);
    font.setItalic(italic);
    textEdit->setFont(font);
    QString style{ "color:%1;background:transparent;margin:1px;padding:2px;caret-color:%1;" };
    style = style.arg(color.name());
    textEdit->textInputCursorColor = color;
    textEdit->setStyleSheet(style);
    connect(textEdit->document(), &QTextDocument::contentsChanged, this, &ShapeText::adjustSize);
    connect(textEdit, &ShapeTextInput::focusOut, this, &ShapeText::focusOut);
	connect(textEdit, &ShapeTextInput::focusIn, this, &ShapeText::focusIn);
}

void ShapeText::focusOut()
{
    auto text = textEdit->document()->toPlainText().trimmed();
    if (text.isEmpty()) {
        state = ShapeState::temp;
    }
    state = ShapeState::ready;
    auto win = (WinBox*)parent();
    win->refreshBoard();
    textEdit->hide();
}

void ShapeText::focusIn()
{
    auto win = (WinBox*)parent();
    win->refreshBoard();
    win->refreshCanvas(nullptr, true);
}

void ShapeText::paint(QPainter* painter)
{
    if (!textEdit->isVisible()) {
        this->textEdit->render(painter, textEdit->pos());
    }   
}
void ShapeText::paintDragger(QPainter* painter)
{
    if (!textEdit->isVisible()) {
        paintBorder(painter);
    }
}
void ShapeText::paintBorder(QPainter* painter)
{
    QPen pen;
    pen.setColor(color);
    pen.setStyle(Qt::DashLine);
    pen.setDashOffset(1);
    pen.setDashPattern({ 1, 2 });
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    auto rect = textEdit->geometry().adjusted(4,4,-4,-4);
    painter->drawRect(rect);
}
void ShapeText::mouseMove(QMouseEvent* event)
{
    auto pos = event->pos();
    auto rect = textEdit->geometry();
    auto innerRect = rect.adjusted(3, 3, -3, -3);
    hoverDraggerIndex = -1;
    if (rect.contains(pos)) {
        auto win = (WinBox*)parent();
        if (innerRect.contains(pos)) {
            hoverDraggerIndex = 0;
            win->updateCursor(Qt::IBeamCursor);
        }
        else {
            hoverDraggerIndex = 8;
            win->updateCursor(Qt::SizeAllCursor);
        }
        win->refreshCanvas(this);
        event->accept();
    }
}
void ShapeText::mousePress(QMouseEvent* event)
{
    if (!textEdit) {
        createTextEdit();
        textEdit->move(event->pos() + QPoint(-10, -10));
        textEdit->show();
        textEdit->setFocus();
        textEdit->setText("");
        event->accept();
        return;
    }
    if (hoverDraggerIndex == 0) {
        textEdit->show();
        textEdit->setFocus();
        auto cursorPos = textEdit->mapFromGlobal(event->pos());
		auto cursor = textEdit->cursorForPosition(cursorPos);
		textEdit->setTextCursor(cursor);
        event->accept();
    }else if(hoverDraggerIndex == 8){
        textEdit->show();
        textEdit->setFocus();
		pressPos = event->pos();
		state = ShapeState::moving;
		event->accept();
	}
}
void ShapeText::mouseRelease(QMouseEvent* event)
{
    if (state >= ShapeState::sizing0) {
        state = ShapeState::ready;
        auto win = (WinBox*)parent();
        win->refreshCanvas(this,true);
        event->accept();
    }
}
void ShapeText::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::moving) {
        auto pos = event->pos();
        auto span = pos - pressPos;
        auto p = textEdit->pos() + span;
		textEdit->move(p.x(), p.y());
        pressPos = pos;
        auto win = (WinBox*)parent();
        win->refreshCanvas(this, true);
        event->accept();
    }
}
