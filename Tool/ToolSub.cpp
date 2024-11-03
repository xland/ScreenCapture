#include <tuple>

#include "../App/App.h"
#include "ToolSub.h"
#include "ToolMain.h"
#include "StrokeCtrl.h"
#include "ColorCtrl.h"
#include "../Win/WinBase.h"

namespace {
	std::map<State, std::vector<ToolBtn>> btns;
	std::vector<std::tuple<int, int, int>> btnSpanIndexs;
}

ToolSub::ToolSub(QWidget *parent) : QWidget(parent)
{
	strokeCtrl = new StrokeCtrl(this);
	colorCtrl = new ColorCtrl(this);
	setCursor(Qt::PointingHandCursor);
	setMouseTracking(true);
	setVisible(false);
	setAttribute(Qt::WA_Hover);  //enterEvent 和 leaveEvent，以及 hoverMoveEvent
}

ToolSub::~ToolSub()
{
}

void ToolSub::InitData(const QJsonObject& obj, const QString& lang)
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

bool ToolSub::getSelectState(const QString& btnName)
{
	auto full = (WinBase*)parent();;
	auto& values = btns[full->state];
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
	auto font = App::getIconFont();
	font->setPixelSize(15);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::TextAntialiasing, true);
	painter.setFont(*font);

	painter.setPen(Qt::GlobalColor::white);
	painter.setBrush(Qt::GlobalColor::white);
	painter.drawRect(rect().adjusted(0, 10, 0, 0));

	auto full = (WinBase*)parent();
	auto index = full->toolMain->selectIndex;
	auto x = 4 + index * 32 + 32 / 2;
	QPolygon triangle;
	triangle << QPoint(x, 6) << QPoint(x-4, 10) << QPoint(x+4, 10);
	painter.drawPolygon(triangle);

	auto& values = btns[full->state];
	x = 4;
	for (size_t i = 0; i < values.size(); i++)
	{
		if (values[i].name == "colorCtrl") {
			x += colorCtrl->width();
		}
		else if (values[i].name == "strokeCtrl") {
			x += strokeCtrl->width();
		}
		else{
			QRect rect(x, 10, btnW, 32);
			if (values[i].selected) {
				rect.adjust(2, 4, -2, -4);
				painter.setPen(Qt::NoPen);
				painter.setBrush(QColor(228, 238, 255));
				painter.drawRoundedRect(rect, 6, 6);
				painter.setPen(QColor(9, 88, 217));
			}
			else if (hoverIndex == i) {
				rect.adjust(2, 4, -2, -4);
				painter.setPen(Qt::NoPen);
				painter.setBrush(QColor(228, 238, 255));
				painter.drawRoundedRect(rect, 6, 6);
				painter.setPen(QColor(33, 33, 33));
			}
			else {
				painter.setPen(QColor(33, 33, 33));
			}			
			painter.drawText(rect, Qt::AlignCenter, values[i].icon);
			x += btnW;
		}
	}
}

void ToolSub::mousePressEvent(QMouseEvent* event)
{
	auto full = (WinBase*)parent();
	auto& values = btns[full->state];
	values[hoverIndex].selected = !values[hoverIndex].selected;
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
		update();
	}
}

void ToolSub::leaveEvent(QEvent* event)
{
	if (hoverIndex != -1) {
		hoverIndex = -1;
		update();
	}
}

void ToolSub::showEvent(QShowEvent* event)
{
	auto full = (WinBase*)parent();
	auto values = btns[full->state];
	auto w{ 4 };
	bool strokeFlag{ false }, colorFlag{ false };
	for (size_t i = 0; i < values.size(); i++)
	{
		if (values[i].name == "colorCtrl") {
			colorCtrl->selectedIndex = values[i].selectedIndex;
			colorCtrl->move(w, 10);
			colorCtrl->show();
			w += colorCtrl->width();
			colorFlag = true;
		}
		else if (values[i].name == "strokeCtrl") {
			strokeCtrl->setMaximum(values[i].max);
			strokeCtrl->setMinimum(values[i].min);
			strokeCtrl->setValue(values[i].value);
			strokeCtrl->move(w, 10);
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
	setFixedSize(w, 42);
	if (!strokeFlag) {
		strokeCtrl->hide();
	}
	if (!colorFlag) {
		colorCtrl->hide();
	}
	QWidget::showEvent(event);
}

std::vector<ToolBtn> ToolSub::makeBtns(const QJsonArray& arr, const QString& lang)
{
	std::vector<ToolBtn> btns;
	for (const QJsonValue& val:arr)
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
