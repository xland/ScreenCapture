#include <qlayout.h>
#include <qpushbutton.h>
#include <qtooltip.h>

#include "ToolBase.h"
#include "../App/App.h"
#include "../App/Font.h"

#include "../Win/Box.h"

namespace
{
    std::vector<ToolBtn> btnsMain;
    std::vector<unsigned> spliterIndexs;
    std::map<State, std::vector<ToolBtn>> btnsSub;
    std::vector<std::tuple<int, int, int>> btnSpanIndexs;
}

ToolBase::ToolBase(Box* box, QWidget* parent) : QWidget(parent), box{ box }
{
    setMouseTracking(true);
    setVisible(false);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_Hover);
    setCursor(Qt::PointingHandCursor);
    //setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    setWindowFlag(Qt::WindowDoesNotAcceptFocus);   //右键Pin菜单，要点两次才会出现
}
ToolBase::~ToolBase()
{
}


void ToolBase::enterEvent(QEnterEvent* event)
{
    QGuiApplication::setOverrideCursor(Qt::PointingHandCursor);
    QWidget::enterEvent(event);
}

void ToolBase::leaveEvent(QEvent* event)
{
    if (hoverIndex != -1)
    {
        hoverIndex = -1;
        update();
    }
    QGuiApplication::restoreOverrideCursor();
    QWidget::leaveEvent(event);
}

void ToolBase::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    //用qt的setFocus始终无法使ToolSub聚焦
    auto wid = (HWND)winId();
    SetFocus(wid);
}

void ToolBase::hideEvent(QHideEvent* event)
{
    selectIndex = -1;
    hoverIndex = -1;
}

std::shared_ptr<QPainter> ToolBase::getPainter()
{
    auto font = Font::icon();
    font->setPixelSize(15);
    auto painter = std::make_shared<QPainter>(this);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->setFont(*font);
    QPen pen;
    pen.setColor(QColor(22, 118, 255));
    pen.setWidthF(border);
    painter->setPen(pen);
    painter->setBrush(Qt::white);
    return painter;
}

void ToolBase::paintBtn(const QChar& icon, const QColor& frontColor, const QColor& bgColor, QRect& rect,QPainter* p)
{
    rect.adjust(2, 4, -2, -4);
    p->setPen(Qt::NoPen);
    p->setBrush(bgColor);
    p->drawRoundedRect(rect, 6, 6);
    p->setPen(frontColor);
    p->drawText(rect, Qt::AlignCenter, icon);
}
void ToolBase::keyPressEvent(QKeyEvent* event)
{
    event->ignore();
    if (event->key() == Qt::Key_Escape) {
        box->escPress();
    }
    else if (event->key() == Qt::Key_Delete) {
        box->removeShape();
    }
    else if (event->key() == Qt::Key_Left) {
        box->moveByKey(0);
    }
    else if (event->key() == Qt::Key_Up) {
        box->moveByKey(1);
    }
    else if (event->key() == Qt::Key_Right) {
        box->moveByKey(2);
    }
    else if (event->key() == Qt::Key_Down) {
        box->moveByKey(3);
    }
    else if (event->modifiers() & Qt::ControlModifier) {
        if (event->key() == Qt::Key_T) {
            box->hideTools();
        }
        else if (event->key() == Qt::Key_Z) {
            box->undo();
        }
        else if (event->key() == Qt::Key_Y) {
            box->redo();
        }
        else if (event->key() == Qt::Key_S) {
            box->saveToFile();
        }
        else if (event->key() == Qt::Key_C) {
            box->saveToClipboard();
        }
        else if (event->key() == Qt::Key_H) {
            box->copyColor(0);
        }
        else if (event->key() == Qt::Key_R) {
            box->copyColor(1);
        }
        else if (event->key() == Qt::Key_K) {
            box->copyColor(2);
        }
        else if (event->key() == Qt::Key_P) {
            box->copyColor(3);
        }
    }
}