#include "ToolSub.h"
#include "CanvasWidget.h"

namespace {
	std::unique_ptr<ToolSub> toolSub;
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
	toolSub = std::make_unique<ToolSub>();
}

void ToolSub::Get()
{
}
