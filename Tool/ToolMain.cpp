#include <qlayout.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <QWindow>

#include "../App/App.h"
#include "../App/Util.h"
#include "../App/NativeRect.h"
#include "../App/Lang.h"
#include "../Win/WinBox.h"
#include "../Win/WinPin.h"
#include "../Win/WinFull.h"
#include "../Win/WinMask.h"
#include "ToolMain.h"
#include "ToolSub.h"
#include "Btn.h"
#include "BtnCheck.h"

ToolMain::ToolMain(WinBox* win) : ToolBase(win)
{
    setFixedSize(14 * btnW + 8, 32);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->addWidget(new BtnCheck("rect", QChar(0xe8e8), State::rect, this));
    layout->addWidget(new BtnCheck("ellipse", QChar(0xe6bc), State::ellipse, this));
	layout->addWidget(new BtnCheck("arrow", QChar(0xe603), State::arrow, this));
	layout->addWidget(new BtnCheck("number", QChar(0xe776), State::number, this));
	layout->addWidget(new BtnCheck("line", QChar(0xe601), State::line, this));
	layout->addWidget(new BtnCheck("text", QChar(0xe6ec), State::text, this));
	layout->addWidget(new BtnCheck("mosaic", QChar(0xe82e), State::mosaic, this));
	layout->addWidget(new BtnCheck("eraser", QChar(0xe6be), State::eraser, this));
    layout->addWidget(new Btn("undo", QChar(0xed85), this));
    layout->addWidget(new Btn("redo", QChar(0xed8a), this));
    layout->addWidget(new Btn("pin", QChar(0xe6a3),this));
    layout->addWidget(new Btn("clipboard", QChar(0xe87f), this));
    layout->addWidget(new Btn("save", QChar(0xe6c0), this));
    layout->addWidget(new Btn("close", QChar(0xe6e7), this));
    setLayout(layout);
}
ToolMain::~ToolMain()
{
}

void ToolMain::setBtnEnable(const QString& name, bool flag)
{
}

void ToolMain::setBtnEnable(bool undo, bool redo)
{
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
    auto screen = NativeRect::getScreenByPos(full->x + full->winMask->maskRect.right(), full->y + full->winMask->maskRect.bottom() + heightSpan);
    if (screen) { //工具条右下角在屏幕内
        topFlag = false;
        //工具条左上角不在屏幕内
        if (!NativeRect::getScreenByPos(left, top)) {
            left = screen->left();
        }
    }
    else { //工具条右下角不在屏幕内
        topFlag = true;
        //判断屏幕顶部是否有足够的空间，工具条是否可以在CutRect右上角
        screen = NativeRect::getScreenByPos(full->x + full->winMask->maskRect.right(), full->y + full->winMask->maskRect.top() - heightSpan);
        if (screen) { //工具条右上角在屏幕内  
            if (NativeRect::getScreenByPos(left, full->y + full->winMask->maskRect.top() - heightSpan)) { //工具条左上角在屏幕内
                top = full->y + full->winMask->maskRect.top() - 6*dpr - height()*dpr;
            }
            else { //工具条左上角不在屏幕中
                left = screen->left();
                top = full->y + full->winMask->maskRect.top() - 6 * dpr - height() * dpr;
            }
        }
        else { //工具条右上角不在屏幕内，此时屏幕顶部和屏幕底部都没有足够的空间，工具条只能显示在截图区域内            
            if (NativeRect::getScreenByPos(left, full->y + full->winMask->maskRect.bottom() - heightSpan)) { //工具条左上角在屏幕内
                top = full->y + full->winMask->maskRect.bottom() - 6*dpr - height()*dpr;
            }
            else { //工具条左上角不在屏幕中，得到截图区域所在屏幕
                screen = NativeRect::getScreenByPos(full->x + full->winMask->maskRect.right(), full->y + full->winMask->maskRect.bottom());
                if (screen) {
                    left = screen->left();
                    top = full->y + full->winMask->maskRect.bottom() - 6*dpr - height()*dpr;
                }
            }
        }
    }
    SetWindowPos(hwnd, nullptr, left, top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void ToolMain::btnCheckChange(BtnCheck* btn)
{
    if (win->toolSub) {
        win->toolSub->close();
    }
    if (!btn->isChecked) {
        return;
    }
    auto btns = findChildren<BtnCheck*>();
	for (auto& b : btns)
	{
		if (b == btn) continue;
        if (b->isChecked) {
			b->isChecked = false;
			b->update();
        }
	}
    win->state = btn->state;
	win->toolSub = new ToolSub(win);
    update();
}

void ToolMain::btnClick(Btn* btn)
{
	if (btn->name == "clipboard") {
		win->saveToClipboard();
	}
	else if (btn->name == "save") {
		win->saveToFile();
	}
	else if (btn->name == "undo") {
		win->undo();
	}
	else if (btn->name == "redo") {
		win->redo();
	}
	else if (btn->name == "pin") {
		WinPin::init((WinFull*)win);
	}
	else if (btn->name == "close") {
		win->close();
	}
}

void ToolMain::paintEvent(QPaintEvent* event)
{
    auto painter = getPainter(); 
    painter->drawRect(rect().toRectF().adjusted(border,border,-border,-border));
    painter->setPen(QColor(190, 190, 190));
    auto x{ 4 + 8 * btnW + 0.5 };
    auto y1{ height() - 9 };
    painter->drawLine(x, 9, x,y1);
    x = 4 + 10 * btnW + 0.5;
    painter->drawLine(x, 9, x, y1);
}

void ToolMain::closeEvent(QCloseEvent* event)
{
    deleteLater();
    win->toolMain = nullptr;
}
