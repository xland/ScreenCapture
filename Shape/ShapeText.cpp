#include <qpainter.h>
#include <QTransform>
#include <QTextDocument>
#include <numbers>
#include <QTextCharFormat>
#include <QTextOption>

#include "ShapeText.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBase.h"
#include "../Layer/Canvas.h"


ShapeText::ShapeText(QObject* parent) : ShapeBase(parent)
{
    auto win = (WinBase*)parent;

    color = win->toolSub->getColor();
    fontSize = win->toolSub->getStrokeWidth();
    bold = win->toolSub->getSelectState("bold");
    italic = win->toolSub->getSelectState("italic");

    textEdit = new ShapeTextInput((QWidget*)parent);
    textEdit->setLineWrapMode(QTextEdit::NoWrap);
    textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁用垂直滚动条
    textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁用水平滚动条
    QFont font = textEdit->font();
    font.setPointSize(fontSize);  // 设置一个较大的字体大小
    textEdit->setFont(font);
    QString style{ "color:%1;background:transparent;margin:1px;padding:2px;" };
    style = style.arg(color.name());
    textEdit->setStyleSheet(style);
    textEdit->setFontItalic(italic);

    if (bold) {
        QFont font = textEdit->font();
        font.setWeight(QFont::Bold);
        textEdit->setFont(font);
    }
    textEdit->hide();
    connect(textEdit->document(), &QTextDocument::contentsChanged, this, &ShapeText::adjustSize);
    connect(textEdit, &ShapeTextInput::focusOut, [this]() {
        textEdit->hide();
        auto win = (WinBase*)this->parent();
        win->update();
        });
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
        size += QSize(12, 20); 
    }
    else {
        size += QSize(6, 2);
        state = ShapeState::ready;
    }
    textEdit->setFixedSize(size);
    textVal = textEdit->toPlainText();
    if (!textEdit->document()->isEmpty()) {
        state = ShapeState::ready;
    }
    auto win = (WinBase*)parent();
    win->canvas->changeShape(this,true);
}

void ShapeText::paint(QPainter* painter)
{
    if (textEdit->isVisible()) {
        return;
    }
    textEdit->render(painter,textEdit->pos());
}
void ShapeText::paintDragger(QPainter* painter)
{
    QPen pen;
    pen.setColor(color);           
    pen.setStyle(Qt::DashLine);    
    pen.setDashOffset(1);          
    pen.setDashPattern({ 1, 2 }); 
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    auto rect = textEdit->geometry();
    painter->drawRect(rect);
}
void ShapeText::mouseMove(QMouseEvent* event)
{
    auto pos = event->pos();
    auto rect = textEdit->geometry();
    auto innerRect = rect.adjusted(2, 2, -2, -2);
    auto outerRect = rect.adjusted(-1, -1, 1, 1);
    hoverDraggerIndex = -1;
    if (outerRect.contains(pos)) {
        auto win = (WinBase*)parent();
        if (innerRect.contains(pos)) {
            hoverDraggerIndex = 0;
            win->updateCursor(Qt::IBeamCursor);
        }
        else {
            hoverDraggerIndex = 8;
            win->updateCursor(Qt::SizeAllCursor);
        }
        win->canvas->changeShape(this,true);
        event->accept();
    }
}
void ShapeText::mousePress(QMouseEvent* event)
{
    if (state == ShapeState::temp) {
        textEdit->move(event->pos() + QPoint(-10,-10));
        textEdit->setText("");
        textEdit->show();
        textEdit->setFocus();
        auto win = (WinBase*)parent();
        win->canvas->changeShape(this,true);
        event->accept();
    }
    else {
        if (textEdit->isHidden() && hoverDraggerIndex == 0) {
            state = ShapeState::temp;
            textEdit->show();
            textEdit->setFocus();
            auto win = (WinBase*)parent();
            win->canvas->changeShape(this, true);
            event->accept();
        }
    }
}
void ShapeText::mouseRelease(QMouseEvent* event)
{
    if (state >= ShapeState::sizing0) {
        QRect rect;
        rect.setCoords(startPos.x(), startPos.y(), endPos.x(), endPos.y());
        //textEdit->move(startPos.toPoint());
        //textEdit->setFixedSize(rect.width(), rect.height());
        //textEdit->setFocus();
        ////textEdit->setPlainText("这是我的文本");
        //textEdit->setHtml("<p style='font-style: italic; text-shadow: 2px 2px 5px gray;'>这是一段带阴影和斜体的文本。</p>");
        //textEdit->raise();
        state = ShapeState::ready;
        auto win = (WinBase*)parent();
        win->canvas->changeShape(this,true);
    }
    event->accept();
}
void ShapeText::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    if (state == ShapeState::sizing0) {
        startPos = event->pos();
    }
    if (state == ShapeState::sizing1) {
        endPos = event->pos();
    }
    if (state == ShapeState::sizing2) {
        endPos = event->pos();
    }
    else if (state == ShapeState::moving) {
        auto pos = event->pos();
        auto span = pos - pressPos;
        startPos+=span;
        endPos+=span;
        pressPos = pos;
    }
    auto win = (WinBase*)parent();
    win->canvas->update();
    event->accept();
}
