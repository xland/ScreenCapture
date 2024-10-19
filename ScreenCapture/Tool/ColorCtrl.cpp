#include <qpainter.h>

#include "ColorCtrl.h"
#include "App.h"

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

void ColorCtrl::paintEvent(QPaintEvent * event)
{
	auto font = App::GetIconFont();
	font->setPixelSize(15);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::TextAntialiasing, true);
	painter.setFont(*font);
	for (int j = 0; j < colorValues.size(); j++)
	{
		QRect rect(j * itemWidth, 0, itemWidth, height());
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
}

void ColorCtrl::showEvent(QShowEvent* event)
{
	selectedIndex = defaultSelectedIndex;
}
