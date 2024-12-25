#include <tuple>
#include <QToolTip>
#include <QPainterPath>

#include "../App/App.h"
#include "ToolSub.h"
#include "ToolMain.h"
#include "StrokeCtrl.h"
#include "ColorCtrl.h"
#include "../Win/WinBox.h"

namespace {
	std::map<State, std::vector<ToolBtn>> btns;
	std::vector<std::tuple<int, int, int>> btnSpanIndexs;
}

ToolSub::ToolSub(WinBox* win) : ToolBase(win)
{
	strokeCtrl = new StrokeCtrl(this);
	colorCtrl = new ColorCtrl(this);
}

ToolSub::~ToolSub()
{
}

void ToolSub::initData(const QJsonObject& obj, const QString& lang)
{
	if (obj["rect"].isArray()) {
		auto arr = obj["rect"].toArray();
		btns.insert({ State::rect,makeBtns(arr, lang) });
	}
	if (obj["ellipse"].isArray()) {
		auto arr = obj["ellipse"].toArray();
		btns.insert({ State::ellipse,makeBtns(arr, lang) });
	}
	if (obj["arrow"].isArray()) {
		auto arr = obj["arrow"].toArray();
		btns.insert({ State::arrow,makeBtns(arr, lang) });
	}
	if (obj["number"].isArray()) {
		auto arr = obj["number"].toArray();
		btns.insert({ State::number,makeBtns(arr, lang) });
	}
	if (obj["line"].isArray()) {
		auto arr = obj["line"].toArray();
		btns.insert({ State::line,makeBtns(arr, lang) });
	}
	if (obj["text"].isArray()) {
		auto arr = obj["text"].toArray();
		btns.insert({ State::text,makeBtns(arr, lang) });
	}
	if (obj["mosaic"].isArray()) {
		auto arr = obj["mosaic"].toArray();
		btns.insert({ State::mosaic,makeBtns(arr, lang) });
	}
	if (obj["eraser"].isArray()) {
		auto arr = obj["eraser"].toArray();
		btns.insert({ State::eraser,makeBtns(arr, lang) });
	}
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
	auto painter = getPainter();
	const int span{ 8 };
	QPainterPath path;
	path.moveTo(border, span);
	path.lineTo(triangleX-4, span);
	path.lineTo(triangleX, 4);
	path.lineTo(triangleX + 4, span);
	path.lineTo(width()- border, span);
	path.lineTo(width()- border, height()- border);
	path.lineTo(border, height()- border);
	path.lineTo(border, span);
	painter->drawPath(path); //有个小三角形指向主工具条的按钮

	auto& values = btns[win->state];
	int x = 4;
	for (int i = 0; i < values.size(); i++)
	{
		if (values[i].name == "colorCtrl") {
			x += colorCtrl->width();
		}
		else if (values[i].name == "strokeCtrl") {
			x += strokeCtrl->width();
		}
		else{
			QRect rect(x, 8, btnW, 32);
			if (values[i].selected) {
				paintBtn(values[i].icon, QColor(9, 88, 217), QColor(228, 238, 255), rect, painter.get());
			}
			else if (hoverIndex == i) {
				paintBtn(values[i].icon, QColor(33, 33, 33), QColor(228, 238, 255), rect, painter.get());
			}
			else {
				paintBtn(values[i].icon, QColor(33, 33, 33), Qt::white, rect, painter.get());
			}
			x += btnW;
		}
	}
	setFocus();
}

void ToolSub::mousePressEvent(QMouseEvent* event)
{
	if (hoverIndex < 0) return;
	auto& values = btns[win->state];
	values[hoverIndex].selected = !values[hoverIndex].selected;
	if (win->state == State::rect ||
		win->state == State::ellipse ||
		win->state == State::mosaic ||
		win->state == State::eraser) {
		if (hoverIndex == 0) {
			strokeCtrl->setEnabled(!values[hoverIndex].selected);
		}
	}
	update();
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
	ToolBase::showEvent(event);
	auto& values = btns[win->state];
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
	}
}

void ToolSub::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Escape) {
		hide();
	}else if (event->key() == Qt::Key_T && (event->modifiers() & Qt::ControlModifier)) {
		win->hideTools();
	}
}
