#include <qlayout.h>
#include <qpushbutton.h>
#include <qtooltip.h>

#include "ToolMain.h"
#include "ToolSub.h"
#include "CanvasWidget.h"
#include "WindowNative.h"
#include "CutMask.h"
#include "App.h"

namespace {
	std::vector<ToolBtn> btns;
	std::vector<unsigned> spliterIndexs;
}

ToolMain::ToolMain(QWidget *parent) : QWidget(parent)
{
	setFixedSize(btns.size()*btnW+8, 32);
	setMouseTracking(true);
	setVisible(false);
	
}
ToolMain::~ToolMain()
{
}

void ToolMain::InitData(const QJsonArray& arr, const QString& lang)
{
	int index{ 0 };
	for (const QJsonValue& val : arr)
	{
		ToolBtn btn;
		auto name = val["name"].toString();
		if (name == "splitter") {
			spliterIndexs.push_back(index);
			continue;
		}
		else if (name == "rect") {
			btn.state = State::rect;
		}
		else if (name == "ellipse") {
			btn.state = State::ellipse;
		}
		else if (name == "arrow") {
			btn.state = State::arrow;
		}
		else if (name == "number") {
			btn.state = State::number;
		}
		else if (name == "line") {
			btn.state = State::line;
		}
		else if (name == "text") {
			btn.state = State::text;
		}
		else if (name == "mosaic") {
			btn.state = State::mosaic;
		}
		else if (name == "eraser") {
			btn.state = State::eraser;
		}
		else {
			btn.name = name;
		}
		btn.tipText = val[lang].toString("");
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
		btns.push_back(btn);
		index += 1;
	}
}

void ToolMain::paintEvent(QPaintEvent * event)
{
	auto font = App::GetIconFont();
	font->setPixelSize(15);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::TextAntialiasing, true);
	painter.setFont(*font);
	painter.setPen(Qt::GlobalColor::white);
	painter.setBrush(Qt::GlobalColor::white);
	painter.drawRect(rect());
	for (size_t i = 0; i < btns.size(); i++)
	{
		QRect rect(4+ i* btnW, 0, btnW, height());
		if (i == selectIndex) {
			rect.adjust(2, 4, -2, -4);
			painter.save();
			painter.setPen(Qt::NoPen);
			painter.setBrush(QColor(228, 238, 255));
			painter.drawRoundedRect(rect, 6, 6);
			painter.restore();
			painter.setPen(QColor(9, 88, 217));
		}else if (i == hoverIndex) {
			rect.adjust(2, 4, -2, -4);
			painter.save();
			painter.setPen(Qt::NoPen);
			painter.setBrush(QColor(238, 238, 238));
			painter.drawRoundedRect(rect,6,6);
			painter.restore();
			painter.setPen(QColor(33, 33, 33));
		}
		else {
			painter.setPen(QColor(33, 33, 33));
		}
		painter.drawText(rect, Qt::AlignCenter,btns[i].icon);
	}
	painter.setPen(QColor(190, 190, 190));
	for (size_t i = 0; i < spliterIndexs.size(); i++)
	{
		painter.drawLine(4+spliterIndexs[i] * btnW+0.5, 9, 4+spliterIndexs[i] * btnW+0.5, height() - 9);
	}
}

void ToolMain::mousePressEvent(QMouseEvent* event)
{
	if (hoverIndex == -1) return;
	auto canvasWidget = CanvasWidget::Get();
	if (hoverIndex == selectIndex) {
		selectIndex = -1;
		canvasWidget->state = State::tool;
	}
	else {
		auto& btn = btns[hoverIndex];
		if (btn.name.isEmpty()) {
			canvasWidget->toolSub->hide();
			selectIndex = hoverIndex;
			canvasWidget->state = btn.state;
			canvasWidget->toolSub->show();
		}
		else if(btn.name == "close"){
			parentWidget()->close();
			WindowNative::Close();
		}		
	}
	update();
	return;
}

void ToolMain::mouseMoveEvent(QMouseEvent* event)
{
	auto x = event->pos().x()-4;
	auto index{ x / (int)btnW };
	if (index >= btns.size())index = -1;
	if (index != hoverIndex) {
		hoverIndex = index;
		update();
		if (hoverIndex>-1) {
			auto pos = event->globalPosition();
			QToolTip::showText(QPoint(pos.x(), pos.y()), btns[hoverIndex].tipText, this);
		}
	}
}

void ToolMain::showEvent(QShowEvent* event)
{
	auto cutMask = CanvasWidget::Get()->cutMask;
	auto pos = cutMask->maskRect.bottomRight();
	move(pos.x() - width(), pos.y() + 6);
	QWidget::showEvent(event);
}

void ToolMain::leaveEvent(QEvent* event)
{
	if (hoverIndex != -1) {
		hoverIndex = -1;
		update();
	}
	QWidget::leaveEvent(event);
}
