#include <qpainter.h>

#include "ColorCtrl.h"

namespace {
	QChar colorIcon;
	QChar colorIconSelected;
	int colorIndex;
	std::vector<QString> colorTips;
	std::vector<QString> colorValues;
}

ColorCtrl::ColorCtrl(QWidget *parent) : QWidget(parent)
{
	auto w{ colorValues.size() * itemWidth };
	setFixedSize(w, 32);
}

ColorCtrl::~ColorCtrl()
{

}

void ColorCtrl::InitData(const QJsonObject& obj, const QString& lang)
{
	colorIndex = obj["defaultSelectedIndex"].toInt();
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

void ColorCtrl::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	for (int j = 0; j < colorValues.size(); j++)
	{
		QRect rect(j * itemWidth, 10, itemWidth, height());
		painter.setPen(QColor(colorValues[j]));
		if (j == colorIndex) {
			painter.drawText(rect, Qt::AlignCenter, colorIconSelected);
		}
		else {
			painter.drawText(rect, Qt::AlignCenter, colorIcon);
		}
	}
}

void ColorCtrl::mousePressEvent(QMouseEvent* event)
{
}
