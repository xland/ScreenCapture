#include "pch.h"
#include "../Win/WinPin.h"
#include "ToolMain.h"
#include "ToolSub.h"

ToolMain::ToolMain(WinPin* win) : Ling::WinBase(), win(win)
{
	auto btnSize{ 32.f * win->dpi };
	// 初始位置由 WinPin::layoutTools() 统一决定，这里只算尺寸
	x = win->x;
	y = win->y + win->h + 5.f * win->dpi;
	for (size_t i = 0; i < btnIds.size(); i++)
	{
		if (btnIds[i] == L"|") {
			w += win->dpi;
		}
		else {
			w += btnSize;
		}
	}
	h = btnSize;
	createNativeWindow(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, WS_POPUP);
}

ToolMain::~ToolMain()
{
}

void ToolMain::init()
{
}

// 返回 curId 对应按钮的中心相对 ToolMain 左边的偏移（物理像素）。
// btnIds 含分隔符而 btns 不含，所以要单独维护 btns 的下标，不能拿 i 去索引 btns。
float ToolMain::getBtnCenterX()
{
	float result{ 0.f };
	size_t btnIndex{ 0 };
	for (size_t i = 0; i < btnIds.size(); i++)
	{
		if (btnIds[i] == L"|") {
			// 分隔符不在 btns 里，宽度与 onCreated 里 spliter 的 setSize(dpi, ...) 一致
			result += dpi;
			continue;
		}
		if (curId == btnIds[i]) {
			result += btns[btnIndex]->w / 2.f;
			return result;
		}
		result += btns[btnIndex]->w;
		btnIndex++;
	}
	return result;
}

void ToolMain::onCreated()
{
	body->setBg(0xFFFFFFFF);
	body->setBorder(1.f, 0xA8A8A8ff);
	body->setAlignItems(Ling::Align::Center);
	body->setFlexDirection(Ling::FlexDirection::Row);
	for (size_t i = 0; i < btnIds.size(); i++)
	{
		auto& id = btnIds[i];
		if (id == L"|") {
			auto spliter = body->makeChild<Ling::Node>();
			spliter->setSize(dpi, 18.f);
			spliter->setBg(0xDDDDDDff);
		}
		else {
			auto btn = body->makeChild<Ling::Button>();
			btn->setId(id);
			btn->setText(btnCodes[i]);
			btn->setHeightPercent(100.f);
			btn->setFlexGrow(1.f);
			btn->setHoverBg(0xF2F2F2ff);
			btn->setFontFamily(L"icon");
			btn->setFontSize(13.f);
			btn->onClick.add([this](Ling::Button* btn) {onClick(btn);});
			btns.push_back(btn);
		}
	}
	show();
}

void ToolMain::applyNormalStyle(Ling::Button* btn)
{
	btn->setBg(0);
	btn->setHoverBg(0xF2F2F2ff);
}

// 取消选中：与 onClick 选中某个按钮是对称操作，只是没有新的选中项。
// ToolSub 由 curId 是否为空驱动，所以清空 curId 后 layoutTools() 会自动把它收起来。
void ToolMain::cancelSelect()
{
	if (curId.empty()) return;
	for (auto b : btns)
	{
		if (b->id == curId) {
			applyNormalStyle(b);
		}
	}
	curId.clear();
	win->toolSub->hideTools();
	// curId 空了 ToolMain 要下移收回 ToolSub 让出的空间，交给 WinPin 重排整组
	win->layoutTools();
}

void ToolMain::onClick(Ling::Button* btn)
{
	for (auto b:btns)
	{
		if (b->id == curId)
		{
			applyNormalStyle(b);
		}
		if (b->id == btn->id)
		{
			b->setBg(0xe6f4ffff);
			b->setHoverBg(0xe6f4ffff);
		}
	}
	curId = btn->id;
	if (curId == L"rect") {
		win->toolSub->showRectTools();
	}
	else if (curId == L"ellipse") {
		win->toolSub->showEllipseTools();
	}
	else if (curId == L"arrow") {
		win->toolSub->showArrowTools();
	}
	else if (curId == L"number") {
		win->toolSub->showNumberTools();
	}
	else if (curId == L"line") {
		win->toolSub->showLineTools();
	}
	else if (curId == L"text") {
		win->toolSub->showTextTools();
	}
	else if (curId == L"mosaic") {
		win->toolSub->showMosaicTools();
	}
	else if (curId == L"eraser") {
		win->toolSub->showEraserTools();
	}
	else {
		win->toolSub->hideTools();
	}
	// curId 变化后 ToolMain 可能要上移给 ToolSub 腾位置，交给 WinPin 重新排布整组
	win->layoutTools();
}

void ToolMain::onMinMaxInfo(MINMAXINFO* mmi)
{
	mmi->ptMinTrackSize.x = 1;
	mmi->ptMinTrackSize.y = 1;
}