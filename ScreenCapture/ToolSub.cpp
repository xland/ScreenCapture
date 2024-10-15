#include "ToolSub.h"
#include "CanvasWidget.h"
#include <memory>

namespace {
	std::unique_ptr<ToolSub> toolSub{nullptr};
}

ToolSub::ToolSub(QWidget *parent) : QMainWindow(parent)
{
	setVisible(false);
}

ToolSub::~ToolSub()
{

}

void ToolSub::Init()
{
	auto parent = CanvasWidget::Get();
	toolSub = std::make_unique<ToolSub>(parent);
}

ToolSub* ToolSub::Get()
{
	return toolSub.get();
}

void ToolSub::Show()
{

}

void ToolSub::InitData(const QJsonObject& obj)
{
}
