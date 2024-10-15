#include <qlayout.h>
#include <qpushbutton.h>
#include <qtooltip.h>

#include "ToolMain.h"
#include "ToolSub.h"
#include "CanvasWidget.h"
#include "CutMask.h"
#include "Config.h"

namespace {
	std::unique_ptr<ToolMain> toolMain;
	std::vector<ToolBtn> btns;
	std::vector<unsigned> spliterIndexs;
}

ToolMain::ToolMain(QWidget *parent) : QWidget(parent)
{
	setFixedSize(btns.size()*btnW+10, 36);
	setMouseTracking(true);
	setVisible(false);
}

ToolMain::~ToolMain()
{}

void ToolMain::Init()
{
	auto parent = CanvasWidget::Get();
	toolMain = std::make_unique<ToolMain>(parent);
}
ToolMain* ToolMain::Get()
{
	return toolMain.get();
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
	auto font = Config::GetIconFont();
	font->setPixelSize(15);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::TextAntialiasing, true);
	painter.setFont(*font);

	QPen pen(QColor(22, 119, 255));
	pen.setWidth(1);    
	painter.setPen(pen);
	painter.setBrush(Qt::GlobalColor::white);
	painter.drawRect(rect().adjusted(1, 1, -1, -1));
	for (size_t i = 0; i < btns.size(); i++)
	{
		QRect rect(5+ i* btnW, 0, btnW, height());
		if (i == selectIndex) {
			rect.adjust(2, 6, -2, -6);
			painter.save();
			painter.setPen(Qt::NoPen);
			painter.setBrush(QColor(228, 238, 255));
			painter.drawRoundedRect(rect, 6, 6);
			painter.restore();
			painter.setPen(QColor(9, 88, 217));
		}else if (i == hoverIndex) {
			rect.adjust(2, 6, -2, -6);
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
	painter.setPen(QColor(180, 180, 180));
	for (size_t i = 0; i < spliterIndexs.size(); i++)
	{
		painter.drawLine(5+spliterIndexs[i] * btnW, 10, 5+spliterIndexs[i] * btnW, height() - 10);
	}
	painter.end();
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
			selectIndex = hoverIndex;
			canvasWidget->state = btn.state;
			ToolSub::Get()->show();
		}
		else if(btn.name == "close"){
			qApp->quit();
		}
		
	}
	update();
	return;
}

void ToolMain::mouseMoveEvent(QMouseEvent* event)
{
	auto x = event->pos().x()-5;
	auto index{ x / (int)btnW };
	if (index >= btns.size())index = -1;
	if (index != hoverIndex) {
		hoverIndex = index;
		repaint();
		if (hoverIndex>-1) {
			auto pos = event->globalPosition();
			QToolTip::showText(QPoint(pos.x(), pos.y()), btns[hoverIndex].tipText, this);
		}
	}
}

void ToolMain::showEvent(QShowEvent* event)
{
	auto canvas = CutMask::Get();
	auto pos = canvas->maskRect.bottomRight();
	toolMain->move(pos.x() - toolMain->width() + 2, pos.y() + 6);
	toolMain->show();
	QWidget::showEvent(event);
}

void ToolMain::leaveEvent(QEvent* event)
{
	if (hoverIndex != -1) {
		hoverIndex = -1;
		repaint();
	}
	QWidget::leaveEvent(event);
}
