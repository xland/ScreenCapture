#include <QWindow>

#include "../App/App.h"
#include "../App/Util.h"
#include "../App/Lang.h"
#include "../Win/WinLong.h"
#include "../Win/CutMask.h"

#include "ToolLong.h"
#include "Btn.h"

ToolLong::ToolLong(QWidget* parent) : ToolBase(parent)
{
    initWindow();
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(4, 2, 4, 2);
    auto tools = App::getTool();
    if (tools.isEmpty()) {
        initDefaultTool(layout);
        setFixedSize(4 * btnW + 8, 32);
    }
    else {
        int btnCount{0};
        for (size_t i = 0; i < tools.size(); i++)
        {
            auto btn = getTool(tools[i]);
            if (btn)
            {
                layout->addWidget(btn);
                btnCount += 1;
            }
        }
        setFixedSize(btnCount * btnW + 8, 32);
    }
    setLayout(layout);
}
ToolLong::~ToolLong()
{
}

void ToolLong::btnClick(Btn* btn)
{
	if (btn->name == "clipboard") {
        auto win = (WinBase*)parent();
        win->saveToClipboard();
	}
	else if (btn->name == "save") {
        auto win = (WinBase*)parent();
        win->saveToFile();
	}
	else if (btn->name == "pin") {
        auto win = dynamic_cast<WinLong*>(parent());
        if (win) {
            win->pin();
        }
	}
	else if (btn->name == "close") {
        auto win = (WinBase*)parent();
        win->close();
        qApp->exit(1);
	}
}

void ToolLong::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());
}

void ToolLong::closeEvent(QCloseEvent* event)
{
    auto win = (WinBase*)parent();
    deleteLater();
    win->toolMain = nullptr;
}

QWidget* ToolLong::getTool(const QString& toolName)
{
    if (toolName == "pin") {
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

void ToolLong::initDefaultTool(QHBoxLayout* layout)
{
    layout->addWidget(new Btn("pin", QChar(0xe6a3), this));
    layout->addWidget(new Btn("clipboard", QChar(0xe87f), this));
    layout->addWidget(new Btn("save", QChar(0xe6c0), this));
    layout->addWidget(new Btn("close", QChar(0xe6e7), this));
}
