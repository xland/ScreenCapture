#include <qlayout.h>
#include <qpushbutton.h>
#include <qtooltip.h>

#include "../App/App.h"
#include "../Win/WinBox.h"
#include "ToolBase.h"
#include "ToolSub.h"

namespace
{
    std::vector<ToolBtn> btnsMain;
    std::vector<unsigned> spliterIndexs;
    std::map<State, std::vector<ToolBtn>> btnsSub;
    std::vector<std::tuple<int, int, int>> btnSpanIndexs;
}

ToolBase::ToolBase(WinBox* win, QWidget* parent) : QWidget(parent),win{win}
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
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
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

void ToolBase::closeEvent(QCloseEvent* event)
{
    deleteLater();
}

void ToolBase::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    //用qt的setFocus始终无法使ToolSub聚焦
    auto wid = (HWND)winId();
    SetFocus(wid);
}

std::shared_ptr<QPainter> ToolBase::getPainter()
{
    auto font = App::getIconFont();
    font->setPixelSize(15);
    auto painter = std::make_shared<QPainter>(this);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->setFont(*font);
    painter->setPen(Qt::white);
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
