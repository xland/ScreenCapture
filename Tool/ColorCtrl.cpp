#include <QToolTip>

#include "../App/Lang.h"
#include "../App/App.h"
#include "../App/Util.h"
#include "ColorCtrl.h"


ColorCtrl::ColorCtrl(int selectIndex, QWidget* parent):selectedIndex{selectedIndex}
{
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_Hover);
	setMouseTracking(true);
	setFocusPolicy(Qt::NoFocus);
	int w{ 8 * itemWidth };
	setFixedSize(w, 28);
}

ColorCtrl::~ColorCtrl()
{

}

QColor ColorCtrl::getColor()
{
    return QColor(colorValues[selectedIndex]);
}

void ColorCtrl::paintEvent(QPaintEvent * event)
{
	auto font = Util::getIconFont(15);
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::TextAntialiasing, true);
	painter.setFont(*font);
	std::vector<QString> colorValues{"#CF1322","#D48806","#389E0D","#13C2C2","#0958D9","#722ED1","#EB2F96","#000000"};
	for (int j = 0; j < colorValues.size(); j++)
	{
		QRect rect(j * itemWidth, 0, itemWidth, height());
		if (j == hoverIndex) {
			rect.adjust(0, 2, 0, -2);
			painter.setPen(Qt::NoPen);
			painter.setBrush(QColor(228, 238, 255));
			painter.drawRoundedRect(rect, 6, 6);
		}
		painter.setPen(QColor(colorValues[j]));
		if (j == selectedIndex) {
			painter.drawText(rect, Qt::AlignCenter, QChar(0xe721));
		}
		else {
			painter.drawText(rect, Qt::AlignCenter, QChar(0xe61d));
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
		if (hoverIndex > -1)
		{
			std::vector<QString> colorNames{"red","yellow","green","cyan","blue","purple","pink","black"};
			QToolTip::showText(QCursor::pos(), Lang::get(colorNames[hoverIndex]), this);
		}
	}
}

void ColorCtrl::leaveEvent(QEvent* event)
{
	if (hoverIndex != -1) {
		hoverIndex = -1;
		update();
	}
}
