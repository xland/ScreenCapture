#include <qlayout.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <QWindow>

#include "../App/App.h"
#include "../App/Util.h"
#include "../App/NativeRect.h"
#include "../App/Lang.h"
#include "../Win/WinPin.h"
#include "../Win/WinFull.h"
#include "../Win/CutMask.h"
#include "ToolMain.h"
#include "ToolSub.h"
#include "Btn.h"
#include "BtnCheck.h"

ToolMain::ToolMain(QWidget* parent) : ToolBase(parent)
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
    auto full = (WinFull*)parent();
    auto dpr = windowHandle()->devicePixelRatio();
    auto rectMask = full->cutMask->rectMask;
    auto br = full->mapToGlobal(rectMask.bottomRight());
    auto tr = full->mapToGlobal(rectMask.topRight());
    auto tl = full->mapToGlobal(rectMask.topLeft());
    auto left { br.x() - width() };
    auto top { br.y() + 6 };
    auto heightSpan{ 6 * 3 + height() * 2 }; //三个缝隙，两个高度
    QList<QScreen*> screens = QGuiApplication::screens();
    auto getScreen = [&screens](int x, int y) {
        for (auto& s : screens)
        {
            if (s->geometry().contains(x, y)) {
                return s;
            }
        }
        };
    auto screen = getScreen(br.x(), br.y() + heightSpan); //1. 工具条在截图区域右下角
    if (screen) { //工具条右下角在屏幕中
        if (!getScreen(left, top)) { //工具条左上角不在屏幕中
            left = screen->geometry().left();
        }
    }
    else { //工具条右下角不在屏幕中
        screen = getScreen(tr.x(), tr.y() - heightSpan); //2. 工具条在截图区域右上角
        if (screen) {
            top = tr.y() - height() - 6;
            if (!getScreen(left, tr.y() - heightSpan)) {//工具条左上角不在屏幕中
                left = screen->geometry().left();
            }
        }
        else { //3. 屏幕顶部和屏幕底部都没有足够的空间，工具条只能显示在截图区域内  
            top = br.y() - height() - 6;
            screen = getScreen(br.x(), top);
            if (!getScreen(left, top)) {//工具条左上角不在屏幕中
                left = screen->geometry().left();
            }
        }
    }
    move(left, top);
}

void ToolMain::btnCheckChange(BtnCheck* btn)
{
    auto win = (WinBase*)parent();
    if (win->toolSub) {
        win->toolSub->close();
    }
    if (!btn->isChecked) {
        win->state = State::tool;
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
    win->setCursor(Qt::CrossCursor);
	win->toolSub = new ToolSub(win);
    update();
}

void ToolMain::btnClick(Btn* btn)
{
    auto win = (WinBase*)parent();
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
    auto win = (WinBase*)parent();
    win->toolMain = nullptr;
}
