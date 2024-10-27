#include <qpainter.h>
#include <QMouseEvent>

#include "ColorCtrl.h"
#include "../App/App.h"

namespace {
	QChar colorIcon;
	QChar colorIconSelected;
	int defaultSelectedIndex;
	std::vector<QString> colorTips;
	std::vector<QString> colorValues;
}

ColorCtrl::ColorCtrl(QWidget *parent) : QWidget(parent)
{
	setVisible(false);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_Hover);
	setMouseTracking(true);
	auto w{ colorValues.size() * itemWidth };
	setFixedSize(w, 32);
}

ColorCtrl::~ColorCtrl()
{

}

void ColorCtrl::InitData(const QJsonObject& obj, const QString& lang)
{
	defaultSelectedIndex = obj["defaultSelectedIndex"].toInt();
	bool ok;
	uint codePoint = obj["icon"].toString().toUInt(&ok, 16);
	if (ok) {
		colorIcon = QChar(codePoint);
	}
	codePoint = obj["iconSelected"].toString().toUInt(&ok, 16);
	if (ok) {
		colorIconSelected = QChar(codePoint);
	}
	auto arr = obj["items"].toArray();
	for (const QJsonValue& item : arr)
	{
		colorTips.push_back(item[lang].toString());
		colorValues.push_back(item["value"].toString());
	}
}

QColor ColorCtrl::getColor()
{
    return QColor(colorValues[selectedIndex]);
}

void ColorCtrl::paintEvent(QPaintEvent * event)
{
	auto font = App::getIconFont();
	font->setPixelSize(15);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::TextAntialiasing, true);
	painter.setFont(*font);
	for (int j = 0; j < colorValues.size(); j++)
	{
		QRect rect(j * itemWidth, 0, itemWidth, height());
		if (j == hoverIndex) {
			rect.adjust(0, 5, 0, -5);
			painter.setPen(Qt::NoPen);
			painter.setBrush(QColor(228, 238, 255));
			painter.drawRoundedRect(rect, 6, 6);
		}
		painter.setPen(QColor(colorValues[j]));
		if (j == selectedIndex) {
			painter.drawText(rect, Qt::AlignCenter, colorIconSelected);
		}
		else {
			painter.drawText(rect, Qt::AlignCenter, colorIcon);
		}
	}
}

void ColorCtrl::mousePressEvent(QMouseEvent* event)
{
	if (selectedIndex != hoverIndex) {
		selectedIndex = hoverIndex;
		update();
	}
}

void ColorCtrl::mouseMoveEvent(QMouseEvent* event)
{
	auto pos = event->pos();
	auto index = pos.x() / itemWidth;
	if (index != hoverIndex) {
		hoverIndex = index;
		update();
	}
}

void ColorCtrl::leaveEvent(QEvent* event)
{
	if (hoverIndex != -1) {
		hoverIndex = -1;
		update();
	}
}

void ColorCtrl::showEvent(QShowEvent* event)
{
	selectedIndex = defaultSelectedIndex;
}
