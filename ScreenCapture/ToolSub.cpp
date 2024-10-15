#include "ToolSub.h"
#include "CanvasWidget.h"
#include <memory>
#include "Config.h"
#include "ToolMain.h"

namespace {
	//todo 钉住窗口该如何复用这个按钮容器
	std::vector<ToolBtn> rectBtns;
	std::vector<ToolBtn> ellipseBtns;
	std::vector<ToolBtn> arrowBtns;
	std::vector<ToolBtn> numberBtns;
	std::vector<ToolBtn> penBtns;
	std::vector<ToolBtn> textBtns;
	std::vector<ToolBtn> mosaicBtns;
	std::vector<ToolBtn> eraserBtns;
	std::vector<ToolBtn> colorsBtns;
}

ToolSub::ToolSub(QWidget *parent) : QMainWindow(parent)
{
	setVisible(false);
	setMouseTracking(true);
}

ToolSub::~ToolSub()
{

}

void ToolSub::InitData(const QJsonObject& obj, const QString& lang)
{
	if (obj["rect"].isArray()) {
		auto arr = obj["rect"].toArray();
		for (const QJsonValue& val : arr)
		{
			rectBtns.push_back(makeBtn(val,lang));
		}
	}
	else if (obj["ellipse"].isArray()) {
		auto arr = obj["ellipse"].toArray();
		for (const QJsonValue& val : arr)
		{
			ellipseBtns.push_back(makeBtn(val, lang));
		}
	}
	else if (obj["arrow"].isArray()) {
		auto arr = obj["arrow"].toArray();
		for (const QJsonValue& val : arr)
		{
			arrowBtns.push_back(makeBtn(val, lang));
		}
	}
	else if (obj["number"].isArray()) {
		auto arr = obj["number"].toArray();
		for (const QJsonValue& val : arr)
		{
			numberBtns.push_back(makeBtn(val, lang));
		}
	}
	else if (obj["pen"].isArray()) {
		auto arr = obj["pen"].toArray();
		for (const QJsonValue& val : arr)
		{
			penBtns.push_back(makeBtn(val, lang));
		}
	}
	else if (obj["text"].isArray()) {
		auto arr = obj["text"].toArray();
		for (const QJsonValue& val : arr)
		{
			textBtns.push_back(makeBtn(val, lang));
		}
	}
	else if (obj["mosaic"].isArray()) {
		auto arr = obj["mosaic"].toArray();
		for (const QJsonValue& val : arr)
		{
			mosaicBtns.push_back(makeBtn(val, lang));
		}
	}
	else if (obj["eraser"].isArray()) {
		auto arr = obj["eraser"].toArray();
		for (const QJsonValue& val : arr)
		{
			eraserBtns.push_back(makeBtn(val, lang));
		}
	}

}

void ToolSub::paintEvent(QPaintEvent* event)
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
	auto toolMain = CanvasWidget::Get()->toolMain; 
	auto toolSub = CanvasWidget::Get()->toolSub;
	auto pos = toolMain->geometry().bottomLeft();
	toolSub->move(pos.x(), pos.y());
	setFixedSize(120, 36);
	QWidget::showEvent(event);
}

ToolBtn ToolSub::makeBtn(const QJsonValue& val, const QString& lang)
{
	ToolBtn btn;
	btn.name = val["name"].toString();
	if (btn.name == "strokeWidth" || btn.name == "colors") {
		return btn;
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
	return btn;
}
