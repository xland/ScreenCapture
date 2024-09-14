#include <qlayout.h>
#include <qpushbutton.h>
#include <QFont>
#include <QFontDatabase>

#include "CanvasWidget.h"
#include "ToolMain.h"

namespace {
	std::unique_ptr<ToolMain> toolMain;
}

ToolMain::ToolMain(QWidget *parent)
	: QWidget(parent)
{
	setFixedSize(15*btnW+6, 36);
	setMouseTracking(true);

	//QVBoxLayout* layout = new QVBoxLayout(this);
	//QPushButton* button = new QPushButton("Click Me", this);
	//connect(button, &QPushButton::clicked, this, &CanvasWidget::onButtonClicked);
	//layout->addWidget(button);


	//setStyleSheet("background-color: red;");
	//connect(button, &QPushButton::clicked, this, &ToolMain::onButtonClicked);
	//QToolTip::showText(widget->mapToGlobal(position), text, widget);
}

ToolMain::~ToolMain()
{}

void ToolMain::Show(const QPoint & pos)
{
	if (!toolMain) {
		auto canvas = CanvasWidget::Get();
		toolMain = std::make_unique<ToolMain>(canvas);
	}
	toolMain->move(pos.x() - toolMain->width()+2, pos.y()+8);
	toolMain->show();
}

void ToolMain::paintEvent(QPaintEvent * event)
{
	auto font = QFont("iconfont");
	font.setStyleStrategy(QFont::PreferAntialias);
	font.setHintingPreference(QFont::PreferFullHinting);
	font.setPixelSize(16);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::TextAntialiasing, true);
	painter.setFont(font);

	QPen pen(QColor(22, 119, 255));
	pen.setWidth(1);    
	painter.setPen(pen);
	painter.setBrush(Qt::GlobalColor::white);
	painter.drawRect(rect().adjusted(1, 1, -1, -1));

	painter.setPen(QColor(33, 33, 33));
	for (size_t i = 0; i < 15; i++)
	{
		QRect rect(i* btnW+3, 0, btnW, height());
		rect.adjust(4, 6, -4, -6);
		if (i == hoverIndex) {
			painter.save();
			painter.setPen(Qt::NoPen);
			painter.setBrush(QColor(238, 238, 238));
			painter.drawRoundedRect(rect,6,6);
			painter.restore();
		}
		painter.drawText(rect, Qt::AlignCenter,iconCode[i]);
	}
	painter.end();
}

void ToolMain::mousePressEvent(QMouseEvent* event)
{
}

void ToolMain::mouseMoveEvent(QMouseEvent* event)
{
	auto x = event->pos().x();
	auto index{ x / (int)btnW };
	if (index != hoverIndex) {
		hoverIndex = index;
		update();
	}
}

void ToolMain::leaveEvent(QEvent* event)
{
	if (hoverIndex != -1) {
		hoverIndex = -1;
		update();
	}
	QWidget::leaveEvent(event);
}
