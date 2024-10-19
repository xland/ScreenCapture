#include "ToolSub.h"
#include "CanvasWidget.h"
#include <memory>
#include "App.h"
#include "ToolMain.h"
#include "StrokeCtrl.h"

namespace {
	std::map<State, std::vector<ToolBtn>> btns;
	QChar colorIcon;
	QChar colorIconSelected;
	int colorIndex;
	std::vector<QString> colorTips;
	std::vector<QString> colorValues;
}

ToolSub::ToolSub(QWidget *parent) : QWidget(parent)
{
	setVisible(false);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_Hover);
	setMouseTracking(true);
	strokeCtrl = new StrokeCtrl(this);
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
	if (obj["color"].isObject()) {
		auto color = obj["color"].toObject(); 
		colorIndex = color["defaultSelectedIndex"].toInt();
		bool ok;
		uint codePoint = color["icon"].toString().toUInt(&ok, 16);
		if (ok) {
			colorIcon = QChar(codePoint);
		}
		codePoint = color["iconSelected"].toString().toUInt(&ok, 16);
		if (ok) {
			colorIconSelected = QChar(codePoint);
		}
		auto arr = color["items"].toArray();
		for (const QJsonValue& item:arr)
		{
			colorTips.push_back(item[lang].toString());
			colorValues.push_back(item["value"].toString());
		}
	}
}

void ToolSub::paintEvent(QPaintEvent* event)
{


	//painter.setPen(QColor(190, 190, 190));
	//painter.drawLine(x, 10 + 16, x + 80, 10 + 16);

	auto font = App::GetIconFont();
	font->setPixelSize(15);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::TextAntialiasing, true);
	painter.setFont(*font);

	painter.setPen(Qt::GlobalColor::white);
	painter.setBrush(Qt::GlobalColor::white);
	painter.drawRect(rect().adjusted(0, 10, 0, 0));

	auto canvasWidget = CanvasWidget::Get();
	auto index = canvasWidget->toolMain->selectIndex;
	auto x = 4+ index * 32 + 32 / 2;
	QPolygon triangle;
	triangle << QPoint(x, 6) << QPoint(x-4, 10) << QPoint(x+4, 10);
	painter.drawPolygon(triangle);

	auto& values = btns[canvasWidget->state];
	x = 4;
	for (size_t i = 0; i < values.size(); i++)
	{
		if (values[i].name == "colors") {
			for (size_t j = 0; j < colorValues.size(); j++)
			{
				QRect rect(x, 10, 26, 32);
				painter.setPen(QColor(colorValues[j]));
				if (j == colorIndex) {
					painter.drawText(rect, Qt::AlignCenter, colorIconSelected);
				}
				else {
					painter.drawText(rect, Qt::AlignCenter, colorIcon);
				}
				x += 26;
			}
		}
		else if (values[i].name == "strokeWidth") {
			x += 84;
		}
		else{
			QRect rect(x, 10, btnW, 32);
			painter.setPen(QColor(33, 33, 33));
			painter.drawText(rect, Qt::AlignCenter, values[i].icon);
			x += btnW;
		}		
	}
}

void ToolSub::mousePressEvent(QMouseEvent* event)
{
}

void ToolSub::mouseMoveEvent(QMouseEvent* event)
{
}

void ToolSub::leaveEvent(QEvent* event)
{
}

void ToolSub::showEvent(QShowEvent* event)
{
	auto canvasWidget = CanvasWidget::Get();
	auto toolMain = canvasWidget->toolMain;
	auto toolSub = canvasWidget->toolSub;
	auto pos = toolMain->geometry().bottomLeft();
	toolSub->move(pos.x(), pos.y());

	auto values = btns[canvasWidget->state];
	auto w{ 4 };
	bool flag{ false };
	for (size_t i = 0; i < values.size(); i++)
	{
		if (values[i].name == "colors") {
			for (size_t j = 0; j < colorValues.size(); j++)
			{
				w += 26;
			}
		}
		else if (values[i].name == "strokeWidth") {
			strokeCtrl->move(w, 0);
			strokeCtrl->setFixedSize(84, 42);
			strokeCtrl->show();
			w += 84;
			flag = true;
		}
		else {
			w += btnW;
		}
	}
	w += 4;
	setFixedSize(w, 42);
	if (!flag) {
		strokeCtrl->hide();
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
		if (btn.name == "strokeWidth" || btn.name == "colors") {
			btns.push_back(std::move(btn));
			continue;
		}
		btn.tipText = val[lang].toString();
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
