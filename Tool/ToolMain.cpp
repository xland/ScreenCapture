#include <qlayout.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <QWindow>

#include "../App/App.h"
#include "../App/Util.h"
#include "../Win/WinBox.h"
#include "../Win/WinPin.h"
#include "../Win/WinFull.h"
#include "../Win/WinMask.h"
#include "ToolMain.h"
#include "ToolSub.h"

namespace{
    std::vector<ToolBtn> btns;
    std::vector<unsigned> spliterIndexs;
}

ToolMain::ToolMain(WinBox* win) : ToolBase(win)
{
    setFixedSize(btns.size() * btnW + 8, 32);
    for (auto& btn : btns)
    {
        if (btn.name == "undo")
        {
            btn.enable = false;
        }
        if (btn.name == "redo")
        {
            btn.enable = false;
        }
    }    
}
ToolMain::~ToolMain()
{
}

void ToolMain::initData(const QJsonArray& arr, const QString& lang)
{
    int index{ 0 };
    for (const QJsonValue& val : arr)
    {
        ToolBtn btn;
        auto name = val["name"].toString();
        if (name == "splitter")
        {
            spliterIndexs.push_back(index);
            continue;
        }
        else if (name == "rect")
        {
            btn.state = State::rect;
        }
        else if (name == "ellipse")
        {
            btn.state = State::ellipse;
        }
        else if (name == "arrow")
        {
            btn.state = State::arrow;
        }
        else if (name == "number")
        {
            btn.state = State::number;
        }
        else if (name == "line")
        {
            btn.state = State::line;
        }
        else if (name == "text")
        {
            btn.state = State::text;
        }
        else if (name == "mosaic")
        {
            btn.state = State::mosaic;
        }
        else if (name == "eraser")
        {
            btn.state = State::eraser;
        }
        else
        {
            btn.name = name;
        }
        btn.tipText = val[lang].toString("");
        btn.selected = val["selectDefault"].toBool(false);
        {
            bool ok;
            uint codePoint = val["icon"].toString().toUInt(&ok, 16);
            if (ok)
            {
                btn.icon = QChar(codePoint);
            }
            else
            {
                qWarning() << "转换失败";
            }
        }
        btns.push_back(btn);
        index += 1;
    }
}

void ToolMain::setBtnEnable(const QString& name, bool flag)
{
	for (auto& btn : btns)
	{
		if (btn.name == name)
		{
            if (btn.enable != flag) {
				btn.enable = flag;
				update();
            }
			break;
		}
	}
}

void ToolMain::setBtnEnable(bool undo, bool redo)
{
    bool flag{ false };
    for (auto& btn : btns)
    {
        if (btn.name == "undo")
        {
            if (btn.enable != undo) {
                btn.enable = undo;
				flag = true;
            }
        }
        if (btn.name == "redo")
        {
            if (btn.enable != redo) {
                btn.enable = redo;
                flag = true;
            }
        }
    }
    if (flag)
    {
        update();
    }
}

void ToolMain::confirmPos()
{
    auto full = (WinFull*)win;
    auto hwnd = (HWND)winId();
    auto dpr = windowHandle()->devicePixelRatio();
    auto left{full->x + full->winMask->maskRect.right() - width()*dpr};
    auto top{ full->y + full->winMask->maskRect.bottom() + 6*dpr };
    //三个缝隙高度和两个工具条高度
    auto heightSpan = 6 * dpr * 3 + height() * dpr * 2;
    auto screen = App::getScreen(full->x + full->winMask->maskRect.right(), full->y + full->winMask->maskRect.bottom() + heightSpan);
    if (screen) { //工具条右下角在屏幕内
        topFlag = false;
        //工具条左上角不在屏幕内
        if (!App::getScreen(left, top)) {
            left = screen->left();
        }
    }
    else { //工具条右下角不在屏幕内
        topFlag = true;
        //判断屏幕顶部是否有足够的空间，工具条是否可以在CutRect右上角
        screen = App::getScreen(full->x + full->winMask->maskRect.right(), full->y + full->winMask->maskRect.top() - heightSpan);
        if (screen) { //工具条右上角在屏幕内  
            if (App::getScreen(left, full->y + full->winMask->maskRect.top() - heightSpan)) { //工具条左上角在屏幕内
                top = full->y + full->winMask->maskRect.top() - 6*dpr - height()*dpr;
            }
            else { //工具条左上角不在屏幕中
                left = screen->left();
                top = full->y + full->winMask->maskRect.top() - 6 * dpr - height() * dpr;
            }
        }
        else { //工具条右上角不在屏幕内，此时屏幕顶部和屏幕底部都没有足够的空间，工具条只能显示在截图区域内            
            if (App::getScreen(left, full->y + full->winMask->maskRect.bottom() - heightSpan)) { //工具条左上角在屏幕内
                top = full->y + full->winMask->maskRect.bottom() - 6*dpr - height()*dpr;
            }
            else { //工具条左上角不在屏幕中，得到截图区域所在屏幕
                screen = App::getScreen(full->x + full->winMask->maskRect.right(), full->y + full->winMask->maskRect.bottom());
                if (screen) {
                    left = screen->left();
                    top = full->y + full->winMask->maskRect.bottom() - 6*dpr - height()*dpr;
                }
            }
        }
    }
    SetWindowPos(hwnd, nullptr, left, top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void ToolMain::paintEvent(QPaintEvent* event)
{
    auto painter = getPainter(); 
    painter->drawRect(rect().toRectF().adjusted(border,border,-border,-border));
    for (int i = 0; i < btns.size(); i++)
    {
        QRect rect(4 + i * btnW, 0, btnW, height());
		if (btns[i].enable)
		{
            if (i == selectIndex) {
                paintBtn(btns[i].icon, QColor(9, 88, 217), QColor(228, 238, 255), rect, painter.get());
            }
            else if (hoverIndex == i) {
                paintBtn(btns[i].icon, QColor(33, 33, 33), QColor(228, 238, 255), rect, painter.get());
            }
            else {
                paintBtn(btns[i].icon, QColor(33, 33, 33), Qt::white, rect, painter.get());
            }
        }
        else {
            paintBtn(btns[i].icon, QColor(180, 180, 180), Qt::white, rect, painter.get());
        }
    }
    painter->setPen(QColor(190, 190, 190));
    for (int i = 0; i < spliterIndexs.size(); i++)
    {
        painter->drawLine(4 + spliterIndexs[i] * btnW + 0.5, 9, 4 + spliterIndexs[i] * btnW + 0.5, height() - 9);
    }
}

void ToolMain::mousePressEvent(QMouseEvent* event)
{
    if (hoverIndex == -1) return;
    if (hoverIndex == selectIndex) //取消选择
    {
        selectIndex = -1;
        win->state = State::tool;
        update();
        win->toolSub->hide();
        if (topFlag) {
            auto pos = geometry().topLeft();
            pos.setY(pos.y() + height()+6);
            move(pos);
        }
    }
    else
    {
        auto& btn = btns[hoverIndex];
        if (btn.name.isEmpty())
        {
            if (topFlag) {
                if (!win->toolSub || !win->toolSub->isVisible()) {
                    auto pos = geometry().topLeft();
                    pos.setY(pos.y() - height() - 6);
                    move(pos);
                }
            }
            win->state = btn.state;
            selectIndex = hoverIndex;
            win->showToolSub();
            update();
        }
        else if (btn.name == "clipboard")
        {
            win->saveToClipboard();
        }
        else if (btn.name == "save")
        {
            win->saveToFile();
        }
        else if (btn.name == "undo")
        {
            if (btn.enable) {
                win->undo();
            }            
        }
        else if (btn.name == "redo")
        {
            if (btn.enable) {
                win->redo();
            }
        }
        else if (btn.name == "pin")
        {
            if (btn.enable) {
				WinPin::init((WinFull*)win);
            }
        }
        else if (btn.name == "close")
        {
            win->close();
        }
    }
}

void ToolMain::mouseMoveEvent(QMouseEvent* event)
{
    auto x = event->pos().x() - 4;
    auto index{ x / (int)btnW };
    if (index >= btns.size())index = -1;
    if (index != hoverIndex)
    {
        hoverIndex = index;
        repaint();
        if (hoverIndex > -1)
        {
            QToolTip::showText(event->globalPosition().toPoint(), btns[hoverIndex].tipText, this);
        }
    }
}
