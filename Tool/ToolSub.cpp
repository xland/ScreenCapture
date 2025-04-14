#include <tuple>
#include <QToolTip>
#include <QPainterPath>
#include <QHBoxLayout>

#include "../App/App.h"
#include "ToolSub.h"
#include "ToolMain.h"
#include "StrokeCtrl.h"
#include "ColorCtrl.h"
#include "../Win/WinBox.h"
#include "BtnCheck.h"

namespace {
	std::map<State, std::vector<ToolBtn>> btns;
	std::vector<std::tuple<int, int, int>> btnSpanIndexs;
}

ToolSub::ToolSub(WinBox* win) : ToolBase(win)
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setContentsMargins(4, 4, 4, 2);
	if (win->state == State::rect) {
		layout->addWidget(new BtnCheck("rectFill", QChar(0xe602), State::noState, this));
		layout->addWidget(new StrokeCtrl(1, 20, 3, this));
		layout->addWidget(new ColorCtrl(0, this));
		setFixedSize(8+btnW + 84+26*8, 40);
		move(win->toolMain->x(), win->toolMain->y() + win->toolMain->height()+2);
		triangleX = 4 + btnW/2;
	}
	else if (win->state == State::ellipse) {
		layout->addWidget(new BtnCheck("ellipseFill", QChar(0xe600), State::noState, this));
		layout->addWidget(new StrokeCtrl(1, 20, 3, this));
		layout->addWidget(new ColorCtrl(1, this));
		setFixedSize(8 + btnW + 84 + 26 * 8, 40);
		move(win->toolMain->x(), win->toolMain->y() + win->toolMain->height() + 2);
		triangleX = 4 + btnW + btnW / 2;
	}
	else if (win->state == State::arrow) {
		layout->addWidget(new BtnCheck("arrowFill", QChar(0xe604), State::noState, this));
		layout->addWidget(new StrokeCtrl(12, 60, 18, this));
		layout->addWidget(new ColorCtrl(2, this));
		setFixedSize(8 + btnW + 84 + 26 * 8, 40);
		move(win->toolMain->x(), win->toolMain->y() + win->toolMain->height() + 2);
		triangleX = 4 + btnW*2 + btnW / 2;
	}
	else if (win->state == State::number) {
		layout->addWidget(new BtnCheck("numberFill", QChar(0xe605), State::noState, this));
		layout->addWidget(new ColorCtrl(3, this));
		setFixedSize(8 + btnW + 26 * 8, 40);
		move(win->toolMain->x(), win->toolMain->y() + win->toolMain->height() + 2);
		triangleX = 4 + btnW * 3 + btnW / 2;
	}
	else if (win->state == State::line) {
		layout->addWidget(new BtnCheck("lineTransparent", QChar(0xe607), State::noState, this));
		layout->addWidget(new StrokeCtrl(6, 160, 12, this));
		layout->addWidget(new ColorCtrl(4, this));
		setFixedSize(8 + btnW + 84 + 26 * 8, 40);
		move(win->toolMain->x(), win->toolMain->y() + win->toolMain->height() + 2);
		triangleX = 4 + btnW * 4 + btnW / 2;
	}
	else if (win->state == State::text) {
		layout->addWidget(new BtnCheck("bold", QChar(0xe634), State::noState, this));
		layout->addWidget(new BtnCheck("italic", QChar(0xe682), State::noState, this));
		layout->addWidget(new StrokeCtrl(8, 160, 16, this));
		layout->addWidget(new ColorCtrl(5, this));
		setFixedSize(8 + btnW*2 + 84 + 26 * 8, 40);
		move(win->toolMain->x(), win->toolMain->y() + win->toolMain->height() + 2);
		triangleX = 4 + btnW * 5 + btnW / 2;
	}
	else if (win->state == State::mosaic) {
		layout->addWidget(new BtnCheck("mosaicFill", QChar(0xe602), State::noState, this));
		layout->addWidget(new StrokeCtrl(16, 80, 26, this));
		auto w{ 8 + btnW + 84 };
		setFixedSize(w, 40);
		auto centerX = 4 + btnW * 6 + btnW / 2;
		triangleX = w / 2;
		move(win->toolMain->x()+ centerX - w/2, win->toolMain->y() + win->toolMain->height() + 2);
	}
	else if (win->state == State::eraser) {
		layout->addWidget(new BtnCheck("eraserFill", QChar(0xe602), State::noState, this));
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

std::vector<ToolBtn> ToolSub::makeBtns(const QJsonArray& arr, const QString& lang)
{
	std::vector<ToolBtn> btns;
	for (const QJsonValue& val : arr)
	{
		ToolBtn btn;
		btn.name = val["name"].toString();
		btn.tipText = val[lang].toString();
		if (btn.name == "strokeCtrl") {
			btn.min = val["min"].toInt();
			btn.max = val["max"].toInt();
			btn.value = val["value"].toInt();
			btns.push_back(std::move(btn));
			continue;
		}
		else if (btn.name == "colorCtrl") {
			btn.selectedIndex = val["selectedIndex"].toInt();
			btns.push_back(std::move(btn));
			continue;
		}
		btn.selected = val["selectDefault"].toBool(false);
		{
			bool ok;
			uint codePoint = val["icon"].toString().toUInt(&ok, 16);
			if (ok) {
				btn.icon = QChar(codePoint);
			}
			else {
				qWarning() << "转换失败";
			}
		}
		btns.push_back(std::move(btn));
	}
	return btns;
}

bool ToolSub::getSelectState(const QString& btnName)
{
	auto& values = btns[win->state];
	auto it = std::find_if(values.begin(), values.end(), [&btnName](const ToolBtn& item) {
			return item.name == btnName;
		});
	if (it != values.end()) {
		return (*it).selected;
	}
	return false;
}

QColor ToolSub::getColor()
{
    return colorCtrl->getColor();
}

int ToolSub::getStrokeWidth()
{
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

void ToolSub::mouseMoveEvent(QMouseEvent* event)
{
	auto x = event->pos().x();
	auto tempIndex{ -1 };
	for (auto& item:btnSpanIndexs)
	{
		auto start{ std::get<0>(item) };
		auto end{ std::get<1>(item) };
		auto index{ std::get<2>(item) };
		if (x >= start && x <= end) {			
			tempIndex = index;
			break;
		}
	}
	if (hoverIndex != tempIndex) {
		hoverIndex = tempIndex;
		if (hoverIndex > -1)
		{
			auto& values = btns[win->state];
			QToolTip::showText(event->globalPosition().toPoint(), values[hoverIndex].tipText, this);
		}
		update();
	}
}

void ToolSub::showEvent(QShowEvent* event)
{
	/*auto& values = btns[win->state];
	auto w{ 4 };
	bool strokeFlag{ false }, colorFlag{ false };
	for (int i = 0; i < values.size(); i++)
	{
		if (values[i].name == "colorCtrl") {
			colorCtrl->selectedIndex = values[i].selectedIndex;
			colorCtrl->move(w, 8);
			colorCtrl->show();
			w += colorCtrl->width();
			colorFlag = true;
		}
		else if (values[i].name == "strokeCtrl") {
			strokeCtrl->setMaximum(values[i].max);
			strokeCtrl->setMinimum(values[i].min);
			strokeCtrl->setValue(values[i].value);
			strokeCtrl->tipInfo = values[i].tipText;
			strokeCtrl->setEnabled(true);
			strokeCtrl->move(w, 8);
			strokeCtrl->show();
			w += strokeCtrl->width();
			strokeFlag = true;
		}
		else {
			btnSpanIndexs.push_back({ w,w + btnW,i });
			w += btnW;
		}
	}
	w += 4;
	setFixedSize(w, 40);
	if (!strokeFlag) {
		strokeCtrl->hide();
	}
	if (!colorFlag) {
		colorCtrl->hide();
	}

	auto pos = win->toolMain->geometry().bottomLeft();
	auto index = win->toolMain->selectIndex;
	auto x = 4 + index * 32 + 32 / 2;
	if (x > w) {
		triangleX = w / 2;
		move(pos.x()+x-w/2, pos.y());
	}
	else {
		triangleX = x;
		move(pos.x(), pos.y());
	}
	if (win->state == State::rect || 
		win->state == State::ellipse || 
		win->state == State::mosaic ||
		win->state == State::eraser) {
		strokeCtrl->setEnabled(!values[0].selected);
	}*/
}

void ToolSub::closeEvent(QCloseEvent* event)
{
	deleteLater();
	win->toolSub = nullptr;
}
