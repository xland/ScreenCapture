#include "pch.h"
#include "WindowMain.h"
#include "TitleBar.h"
#include "WebSocket.h"
#include "ImgViewer.h"
#include "TextBox.h"
#include "StatusBar.h"

namespace {
    static std::unique_ptr<WindowMain> ins;
}
WindowMain::WindowMain() : Ling::WinBase()
{
    setTitle(L"图像识别工具");
    setSize(800, 600);
    setCenter();
    onDestroy.add([this] { Ling::App::get()->quit(); });
	onSizeChanged.add([this] {  scrollerBox->setHeight(h/dpi- 30 - 28); });
    createNativeWindow();
}

WindowMain::~WindowMain()
{
}

void WindowMain::init()
{
    Ling::init();
    Ling::D2D::get()->addFonts({ L"icon.ttf" });
    auto ptr = new WindowMain();
    ins.reset(ptr);
}

void WindowMain::onCreated()
{
    enableShadow();
    body->setBg(0xFFFFFFFF);
    body->setFlexDirection(Ling::FlexDirection::Column);
    titleBar = body->makeChild<TitleBar>();
    auto content = body->makeChild<Ling::Node>();
    content->setFlexGrow(1.f);
    content->setFlexDirection(Ling::FlexDirection::Row);
    imgViewer = content->makeChild<ImgViewer>();
    splitter = content->makeChild<Ling::Node>();
    splitter->setHeightPercent(100.f);
    splitter->setWidth(3.f);
    splitter->setBg(0xeeeeeeFF);
    scrollerBox = content->makeChild<Ling::ScrollerBox>();
    scrollerBox->setWidth(260.f);
    scrollerBox->setHeight(h / dpi - 30 - 28);
    textBox = scrollerBox->makeChild<TextBox>();
    statusBar = body->makeChild<StatusBar>();
    // 图像侧和文本侧共享 ImgViewer 持有的那份 OcrDoc，任一侧改了选区都从这里回灌两侧
    imgViewer->getDoc()->onSelectionChanged = [this] { onSelectionChanged(); };
    onCursor.add([this](bool* flag) {this->onSetCursor(flag);});
    onMouseDown.add([this](POINT pt, bool isRight) {this->onDown(pt,isRight);});
    onMouseMove.add([this](POINT pt) {this->onMove(pt);});
    onMouseUp.add([this](POINT pt,bool isRight) {this->onUp(pt);});
    onKeyDown.add([this](UINT vk) {this->onKey(vk);});
    show();
}

void WindowMain::onSelectionChanged()
{
    // 图像侧拖拽时把右侧列表滚到选区末端，让联动高亮可见
    if (imgViewer->isSelecting()) textBox->scrollFocusIntoView();
    imgViewer->redrawSelection();
    textBox->redrawSelection();
}

void WindowMain::onKey(UINT vk)
{
    if ((GetKeyState(VK_CONTROL) & 0x8000) == 0) return;
    auto doc = imgViewer->getDoc();
    if (vk == 'C') {
        auto text = doc->getSelectedText();
        if (!text.empty()) Ling::Util::setTextToClipboard(text);
    }
    else if (vk == 'A') {
        if (doc->selectAll()) doc->notifyChanged();
    }
}

LRESULT WindowMain::onHitTest(const POINT pos)
{
    POINT pt = pos;
    ScreenToClient(hwnd, &pt);
    if (!isMaximized) {
        auto result = borderHitTest(pt);
        if (result != HTCLIENT) return result;
    }
    return titleBar->hitCaption(pt);
}

void WindowMain::onSetCursor(bool* flag)
{
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hwnd, &pt);
    if (splitter->isPosIn(pt) || isDragging) {
        *flag = true;
        SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
        return;
    }
    // 文本可选区域给 I-beam。拖拽中即使鼠标移出去了也保持 I-beam，
    // 否则边缘自动滚动时光标会来回跳。
    if (imgViewer->isSelecting() || textBox->isSelecting()
        || imgViewer->isPosInImage(pt) || textBox->isPosInContent(pt)) {
        *flag = true;
        SetCursor(LoadCursor(nullptr, IDC_IBEAM));
    }
}

void WindowMain::onDown(POINT pt, bool isRight)
{
    if (isRight) return;
    if (!splitter->isPosIn(pt)) return;
    // 落在 splitter 上：开始拖拽
    isDragging = true;
    SetCapture(hwnd);
}

void WindowMain::onMove(POINT pt)
{
    if (!isDragging) return;
    if (pt.x < 200) return;
    auto scrollerBoxW{ (w - pt.x) / dpi };
    if (scrollerBoxW < 120) return;
    scrollerBox->setWidth((w-pt.x)/dpi);
    refresh();
}

void WindowMain::onUp(POINT pt)
{
    if (!isDragging) return;
    isDragging = false;
    ReleaseCapture();
}
