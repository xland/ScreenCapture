#include <tuple>
#include <QToolTip>
#include <QPainterPath>
#include <QHBoxLayout>

#include "../App/App.h"
#include "../Win/WinBase.h"
#include "ToolSub.h"
#include "ToolMain.h"
#include "StrokeCtrl.h"
#include "ColorCtrl.h"
#include "BtnBase.h"
#include "BtnCheck.h"

ToolSub::ToolSub(QWidget* parent) : ToolBase(parent)
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setContentsMargins(4, 4, 4, 2);
	auto win = (WinBase*)parent;
	if (win->state == State::rect) {
		layout->addWidget(new BtnCheck("rectFill", QChar(0xe602), this));
		layout->addWidget(new StrokeCtrl(1, 20, 2, this));
		layout->addWidget(new ColorCtrl(0, this));
		setFixedSize(8+btnW + 84+26*8, 40);
		auto x = win->toolMain->x();
		auto y = win->toolMain->y() + win->toolMain->height() + 2;
		move(x, y);
		triangleX = 4 + btnW/2;
	}
	else if (win->state == State::ellipse) {
		layout->addWidget(new BtnCheck("ellipseFill", QChar(0xe600), this));
		layout->addWidget(new StrokeCtrl(1, 20, 2, this));
		layout->addWidget(new ColorCtrl(0, this));
		setFixedSize(8 + btnW + 84 + 26 * 8, 40);
		move(win->toolMain->x(), win->toolMain->y() + win->toolMain->height() + 2);
		triangleX = 4 + btnW + btnW / 2;
	}
	else if (win->state == State::arrow) {
		layout->addWidget(new BtnCheck("arrowFill", QChar(0xe604), this, State::noState,true));
		layout->addWidget(new StrokeCtrl(12, 60, 18, this));
		layout->addWidget(new ColorCtrl(0, this));
		setFixedSize(8 + btnW + 84 + 26 * 8, 40);
		move(win->toolMain->x(), win->toolMain->y() + win->toolMain->height() + 2);
		triangleX = 4 + btnW*2 + btnW / 2;
	}
	else if (win->state == State::number) {
		layout->addWidget(new BtnCheck("numberFill", QChar(0xe605), this, State::noState,true));
		layout->addWidget(new ColorCtrl(0, this));
		setFixedSize(8 + btnW + 26 * 8, 40);
		move(win->toolMain->x(), win->toolMain->y() + win->toolMain->height() + 2);
		triangleX = 4 + btnW * 3 + btnW / 2;
	}
	else if (win->state == State::line) {
		layout->addWidget(new BtnCheck("lineTransparent", QChar(0xe607), this));
		layout->addWidget(new StrokeCtrl(6, 160, 12, this));
		layout->addWidget(new ColorCtrl(0, this));
		setFixedSize(8 + btnW + 84 + 26 * 8, 40);
		move(win->toolMain->x(), win->toolMain->y() + win->toolMain->height() + 2);
		triangleX = 4 + btnW * 4 + btnW / 2;
	}
	else if (win->state == State::text) {
		layout->addWidget(new BtnCheck("bold", QChar(0xe634), this));
		layout->addWidget(new BtnCheck("italic", QChar(0xe682), this));
		layout->addWidget(new StrokeCtrl(12, 160, 22, this));
		layout->addWidget(new ColorCtrl(0, this));
		setFixedSize(8 + btnW*2 + 84 + 26 * 8, 40);
		move(win->toolMain->x(), win->toolMain->y() + win->toolMain->height() + 2);
		triangleX = 4 + btnW * 5 + btnW / 2;
	}
	else if (win->state == State::mosaic) {
		layout->addWidget(new BtnCheck("mosaicFill", QChar(0xe602), this));
		layout->addWidget(new StrokeCtrl(16, 80, 26, this));
		auto w{ 8 + btnW + 84 };
		setFixedSize(w, 40);
		auto centerX = 4 + btnW * 6 + btnW / 2;
		triangleX = w / 2;
		move(win->toolMain->x()+ centerX - w/2, win->toolMain->y() + win->toolMain->height() + 2);
	}
	else if (win->state == State::eraser) {
		layout->addWidget(new BtnCheck("eraserFill", QChar(0xe602), this));
		layout->addWidget(new StrokeCtrl(12, 60, 16, this));
		auto w{ 8 + btnW + 84 };
		setFixedSize(w, 40);
		auto centerX = 4 + btnW * 7 + btnW / 2;
		triangleX = w / 2;
		move(win->toolMain->x()+ centerX - w / 2, win->toolMain->y() + win->toolMain->height() + 2);
	}
	setLayout(layout);
	show();
}

ToolSub::~ToolSub()
{
}


bool ToolSub::getSelectState(const QString& btnName)
{
	auto btns = findChildren<BtnCheck*>();
	for (auto& btn:btns)
	{
		if (btn->name == btnName) {
			return btn->isChecked;
		}
	}
	return false;
}

QColor ToolSub::getColor()
{
	auto colorCtrl = findChild<ColorCtrl*>();
    return colorCtrl->getColor();
}

int ToolSub::getStrokeWidth()
{
	auto strokeCtrl = findChild<StrokeCtrl*>();
    return strokeCtrl->value();
}

void ToolSub::paintEvent(QPaintEvent* event)
{
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing, true);
	QPainterPath path;
	path.moveTo(border, 5);
	path.lineTo(triangleX-4, 5);
	path.lineTo(triangleX, border);
	path.lineTo(triangleX + 4, 5);
	path.lineTo(width()- border, 5);
	path.lineTo(width()- border, height()- 4);
	path.lineTo(border, height()- 4);
	path.lineTo(border, 5);
	p.setPen(QColor(22, 118, 255));
	p.setBrush(Qt::white);
	p.drawPath(path); //有个小三角形指向主工具条的按钮
}

void ToolSub::closeEvent(QCloseEvent* event)
{
	deleteLater();
	auto win = (WinBase*)parent();
	win->toolSub = nullptr;
}
