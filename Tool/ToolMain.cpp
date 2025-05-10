#include <QWindow>

#include "../App/App.h"
#include "../App/Util.h"
#include "../App/Lang.h"
#include "../Win/WinPin.h"
#include "../Win/WinFull.h"
#include "../Win/CutMask.h"
#include "../Win/Canvas.h"

#include "ToolMain.h"
#include "ToolSub.h"
#include "Btn.h"
#include "BtnCheck.h"

ToolMain::ToolMain(QWidget* parent) : ToolBase(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(4, 2, 4, 2);
    auto tools = App::getTool();
    if (tools.isEmpty()) {
        initDefaultTool(layout);
        setFixedSize(14 * btnW + 8, 32);
    }
    else {
        int btnCount{0};
        for (size_t i = 0; i < tools.size(); i++)
        {
            if (tools[i] == "|") {
                splitters.push_back(i - splitters.size());
            }
            else {
                layout->addWidget(getTool(tools[i])); 
                btnCount += 1;
            }
        }
        setFixedSize(btnCount * btnW + 8, 32);
    }
    setLayout(layout);
}
ToolMain::~ToolMain()
{
}

void ToolMain::setBtnEnable(const QString& name, bool flag)
{
    auto btns = findChildren<Btn*>();
	for (auto& b : btns)
	{
		if (b->name == name) {
			b->setEnable(flag);
			break;
		}
	}
}

void ToolMain::confirmPos()
{
    auto full = (WinFull*)parent();
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
            if (s->geometry().contains(x, y)) return s;
        }
		return (QScreen*)nullptr;
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
            posState = 1;
            if (!getScreen(left, tr.y() - heightSpan)) {//工具条左上角不在屏幕中
                left = screen->geometry().left();
            }
        }
        else { //3. 屏幕顶部和屏幕底部都没有足够的空间，工具条只能显示在截图区域内  
            top = br.y() - height() - 6;
            posState = 2;
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
    else {
        if (posState == 1 || posState == 2) {
            move(x(), y() - height() - 12);
        }
    }
    if (!btn->isChecked) {
        win->state = State::tool;
        win->toolSub = nullptr;
        if (posState == 1 || posState == 2) {
            move(x(), y() + height() + 12);
        }
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
    if (btn->state == State::text) {
        win->setCursor(Qt::IBeamCursor);
    }
    else {
        win->setCursor(Qt::CrossCursor);
    }
    btnCheckedCenterX = btn->x() + btn->width() / 2;
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
		win->canvas->undo();
	}
	else if (btn->name == "redo") {
		win->canvas->redo();
	}
	else if (btn->name == "pin") {
        auto winFull = dynamic_cast<WinFull*>(parent());
        if (winFull) {
            winFull->pin();
        }
	}
	else if (btn->name == "close") {
		win->close();
        qApp->exit(1);
	}
}

void ToolMain::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    auto font = Util::getIconFont(15);
    font->setPixelSize(15);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setFont(*font);
    QPen pen;
    pen.setColor(QColor(22, 118, 255));
    pen.setWidthF(border);
    painter.setPen(pen);
    painter.setBrush(Qt::white);
    painter.drawRect(rect().toRectF().adjusted(border,border,-border,-border));
    painter.setPen(QColor(190, 190, 190));
    auto y1{ height() - 9 };

    for (size_t i = 0; i < splitters.size(); i++)
    {
        auto x{ 4 + splitters[i] * btnW + 0.5 };
        painter.drawLine(x, 9, x, y1);
    }
}

void ToolMain::closeEvent(QCloseEvent* event)
{
    auto win = (WinBase*)parent();
	if (win->toolSub) {
		win->toolSub->close();
	}
    deleteLater();
    win->toolMain = nullptr;
}

QWidget* ToolMain::getTool(const QString& toolName)
{
    if (toolName == "rect") {
        return new BtnCheck("rect", QChar(0xe8e8), this, State::rect);
    }
    else if (toolName == "ellipse") {
        return new BtnCheck("ellipse", QChar(0xe6bc), this, State::ellipse);
    }
    else if (toolName == "arrow") {
        return new BtnCheck("arrow", QChar(0xe603), this, State::arrow);
    }
    else if (toolName == "number") {
        return new BtnCheck("number", QChar(0xe776), this, State::number);
    }
    else if (toolName == "line") {
        return new BtnCheck("line", QChar(0xe601), this, State::line);
    }
    else if (toolName == "text") {
        return new BtnCheck("text", QChar(0xe6ec), this, State::text);
    }
    else if (toolName == "mosaic") {
        return new BtnCheck("mosaic", QChar(0xe82e), this, State::mosaic);
    }
    else if (toolName == "eraser") {
        return new BtnCheck("eraser", QChar(0xe6be), this, State::eraser);
    }
    else if (toolName == "undo") {
        return new Btn("undo", QChar(0xed85), false, this);
    }
    else if (toolName == "redo") {
        return new Btn("redo", QChar(0xed8a), false, this);
    }
    else if (toolName == "pin") {
        return new Btn("pin", QChar(0xe6a3), this);
    }
    else if (toolName == "clipboard") {
        return new Btn("clipboard", QChar(0xe87f), this);
    }
    else if (toolName == "save") {
        return new Btn("save", QChar(0xe6c0), this);
    }
    else if (toolName == "close") {
        return new Btn("close", QChar(0xe6e7), this);
    }
    return nullptr;
}

void ToolMain::initDefaultTool(QHBoxLayout* layout)
{
    layout->addWidget(new BtnCheck("rect", QChar(0xe8e8), this, State::rect));
    layout->addWidget(new BtnCheck("ellipse", QChar(0xe6bc), this, State::ellipse));
    layout->addWidget(new BtnCheck("arrow", QChar(0xe603), this, State::arrow));
    layout->addWidget(new BtnCheck("number", QChar(0xe776), this, State::number));
    layout->addWidget(new BtnCheck("line", QChar(0xe601), this, State::line));
    layout->addWidget(new BtnCheck("text", QChar(0xe6ec), this, State::text));
    layout->addWidget(new BtnCheck("mosaic", QChar(0xe82e), this, State::mosaic));
    layout->addWidget(new BtnCheck("eraser", QChar(0xe6be), this, State::eraser));
    layout->addWidget(new Btn("undo", QChar(0xed85), false, this));
    layout->addWidget(new Btn("redo", QChar(0xed8a), false, this));
    layout->addWidget(new Btn("pin", QChar(0xe6a3), this));
    layout->addWidget(new Btn("clipboard", QChar(0xe87f), this));
    layout->addWidget(new Btn("save", QChar(0xe6c0), this));
    layout->addWidget(new Btn("close", QChar(0xe6e7), this));
}
