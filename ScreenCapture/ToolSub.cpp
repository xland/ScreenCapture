#include "ToolSub.h"
#include "CanvasWidget.h"
#include <memory>

namespace {
	std::unique_ptr<ToolSub> toolSub{nullptr};
}

ToolSub::ToolSub(QWidget *parent) : QMainWindow(parent)
{
}

ToolSub::~ToolSub()
{

}

void ToolSub::Init()
{
	auto canvasWidget = CanvasWidget::Get();
	toolSub = std::make_unique<ToolSub>(canvasWidget);
}

ToolSub* ToolSub::Get()
{
	return toolSub.get();
}
