#include "pch.h"
#include "Util.h"
#include "WinToolBase.h"
#include "WinToolSub.h"
#include "WinToolMain.h"
#include "WinPin.h"
#include "History.h"

std::unique_ptr<WinToolSub> winToolSub;

WinToolSub::WinToolSub() :WinToolBase(0, 0, 0, 0)
{
}

WinToolSub::~WinToolSub()
{

}

void WinToolSub::initVal()
{
    auto toolMain = WinToolMain::get();
    marginTop =  4.f * toolMain->dpi;
    auto span{ 4.f * toolMain->dpi };
    y = toolMain->y + toolMain->h + 3.f * toolMain->dpi;
    h = int(toolMain->h + marginTop);
    btnSize = toolMain->h;
    colorBtnW = toolMain->dpi * 23;
    if (toolMain->state == "rect" || toolMain->state == "ellipse") {
        w = 316 * toolMain->dpi;
        btnStart = 0;
        btnEnd = btnStart + btnSize;
        sliderMax = 16;
        sliderMin = 1;
        sliderVal = 3;
        sliderStart = btnEnd + span;
        sliderEnd = sliderStart + 120.f;
        colorStart = sliderEnd + span;
        colorEnd = w;
    }
    else if (toolMain->state == "arrow") {
        w = 316 * toolMain->dpi;
        btnStart = 0;
        btnEnd = btnStart + btnSize;
        sliderMax = 16;
        sliderMin = 1;
        sliderVal = 3;
        selectIndex = 0;
        sliderStart = btnEnd + span;
        sliderEnd = sliderStart + 120.f;
        colorStart = sliderEnd + span;
        colorEnd = w;
    }
    else if (toolMain->state == "line") {
        w = (int)(316 * toolMain->dpi);
        btnStart = 0;
        btnEnd = btnStart + btnSize;
        sliderMax = 60;
        sliderMin = 1;
        sliderVal = 3;
        sliderStart = btnEnd + span;
        sliderEnd = sliderStart + 120.f;
        colorStart = sliderEnd + span;
        colorEnd = w;
    }
    else if (toolMain->state == "number") {
        w = (int)(228 * toolMain->dpi);
        btnStart = 0;
        btnEnd = btnStart + btnSize;
        sliderStart = 0;
        sliderEnd = 0;
        selectIndex = 0;
        colorStart = btnEnd ;
        colorEnd = w;
    }
    else if (toolMain->state == "text") {
        w = (int)(348 * toolMain->dpi);
        btnStart = 0;
        btnEnd = btnStart + btnSize*2;
        sliderMax = 22;
        sliderMin = 12;
        sliderVal = 16;
        sliderStart = btnEnd + span;
        sliderEnd = sliderStart + 120.f;
        colorStart = sliderEnd + span;
        colorEnd = w;
    }
    else if (toolMain->state == "mosaic" || toolMain->state == "eraser") {
        w = 126 * toolMain->dpi;
        btnStart = 0;
        btnEnd = btnStart + btnSize;
        sliderMax = 22;
        sliderMin = 12;
        sliderVal = 16;
        sliderStart = btnEnd + span;
        sliderEnd = sliderStart + 120.f;
    }
    arrowX = btnSize * toolMain->selectIndex + btnSize / 2;
    if (arrowX + 6.f * toolMain->dpi > w)
    {
        x = toolMain->x + arrowX - w / 2; // 箭头顶点位于子工具条正中间
        arrowX = w / 2.f;
    }
    else
    {
        x = toolMain->x; // 子工具条左侧与主工具条左侧对齐
    }
}

void WinToolSub::initBtn()
{
    btnId = { "rectFill" , "ellipseFill", "arrowFill", "numberFill" , "lineTransparent" ,"bold" , "italic", "check", "uncheck" };
    btnName = { L"矩形填充",L"圆形填充",L"箭头填充",L"标号填充",L"半透明",L"粗体",L"斜体" };
    auto fontSize{ 14.f * dpi };
    btnLayout = {
        getIconLayout(L"\ue602", fontSize, btnSize, btnSize),
        getIconLayout(L"\ue600", fontSize, btnSize, btnSize),
        getIconLayout(L"\ue604", fontSize, btnSize, btnSize),
        getIconLayout(L"\ue605", fontSize, btnSize, btnSize),
        getIconLayout(L"\ue607", fontSize, btnSize, btnSize),
        getIconLayout(L"\ue634", fontSize, btnSize, btnSize),
        getIconLayout(L"\ue682", fontSize, btnSize, btnSize),
        getIconLayout(L"\ue721", fontSize, btnSize, btnSize),
        getIconLayout(L"\ue61d", fontSize, btnSize, btnSize)
    };
}

bool WinToolSub::hoverBtn(const int& x)
{
    int indexBtn{ -1 };
    if (x > btnStart && x < btnEnd) {
        indexBtn = static_cast<int>((x - btnStart) / btnSize);
    }
    else {
        indexBtn = -1;
    }
    if (indexBtn != hoverIndex) {
        hoverIndex = indexBtn;
        if (indexBtn >= 0) {
            int startIndex = 0; //rect/mosaic/eraser
            auto toolMain = WinToolMain::get();
            if (toolMain->state == "ellipse") startIndex = 1;
            if (toolMain->state == "arrow")startIndex = 2;
            if (toolMain->state == "number")startIndex = 3;
            if (toolMain->state == "line")startIndex = 4;
            if (toolMain->state == "text")startIndex = 5;
            tipText = btnName[startIndex + indexBtn];
            showTipAt((int)(this->x + btnStart + indexBtn * btnSize + btnSize / 2), (int)(this->y + marginTop + 4 * dpi));
        }
        return true;
    }
    return false;
}

bool WinToolSub::hoverColor(const int& x)
{
    int indexColor;
    if (x > colorStart && x < colorEnd) {
        indexColor = static_cast<int>((x - colorStart) / colorBtnW);
        if (indexColor >= colorName.size()) indexColor = (int)colorName.size() - 1;
    }
    else {
        indexColor = -1;
    }
    if (indexColor != hoverColorIndex) {
        hoverColorIndex = indexColor; 
        if (indexColor >= 0) {
            tipText = colorName[indexColor];
            showTipAt(this->x + colorStart + indexColor * colorBtnW + btnSize / 2, this->y + marginTop + 4 * dpi);
        }
        return true;
    }
    return false;
}

void WinToolSub::popup(WinPin* parent)
{
    if (!winToolSub.get()) {
        winToolSub = std::make_unique<WinToolSub>();
        winToolSub->parent = parent;
        winToolSub->initVal();
        winToolSub->createWindow(WS_EX_TOOLWINDOW|WS_EX_TOPMOST| WS_EX_NOACTIVATE);
        winToolSub->initTip();
        winToolSub->initBrush();
        winToolSub->initColor();
        winToolSub->enableAlpha();
        winToolSub->initBtn();
        winToolSub->initBorder();
        winToolSub->show();
    }
    else {
        auto toolMain = WinToolMain::get();
        auto win = winToolSub.get();
        win->parent = parent;
        if (toolMain->state == "") {
            win->hide();
            return;
        }
        win->initVal();
        win->move(win->x, win->y);
        win->resize(win->w, win->h);
        win->initBorder();
        win->show();
        win->refresh();
    }
}
WinToolSub* WinToolSub::get()
{
    return winToolSub.get();
}

D2D1_COLOR_F WinToolSub::getSelectedColor()
{
    return colorBrush[selectColorIndex]->GetColor();
}

void WinToolSub::initBorder()
{
    borderPath.Reset();
    auto d2d = Util::getD2D();
    d2d->CreatePathGeometry(borderPath.GetAddressOf());
    ComPtr<ID2D1GeometrySink> sink;
    borderPath->Open(sink.GetAddressOf());
    auto borderW{ dpi };
    sink->BeginFigure({ borderW,marginTop }, D2D1_FIGURE_BEGIN_FILLED);
    D2D1_POINT_2F points[6] = {
        D2D1_POINT_2F{arrowX - 3.f * dpi,marginTop},
        D2D1_POINT_2F{arrowX,dpi},
        D2D1_POINT_2F{arrowX + 3.f * dpi,marginTop},
        D2D1_POINT_2F{(float)w - borderW,marginTop},
        D2D1_POINT_2F{(float)w - borderW,(float)h - borderW},
        D2D1_POINT_2F{borderW,(float)h - borderW}
    };
    sink->AddLines(points, 6);
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
}

void WinToolSub::initColor()
{
    std::vector<D2D1_COLOR_F> colors = {
        D2D1::ColorF(0XCF1322),
        D2D1::ColorF(0XD48806),
        D2D1::ColorF(0X389E0D),
        D2D1::ColorF(0X13C2C2),
        D2D1::ColorF(0X0958D9),
        D2D1::ColorF(0X722ED1),
        D2D1::ColorF(0XEB2F96),
        D2D1::ColorF(0X000000)
    };
    colorName = {
        L"红",L"黄",L"绿",L"青",L"蓝",L"紫",L"粉",L"黑",
    };
    for (auto& color : colors)
    {
        ComPtr<ID2D1SolidColorBrush> brush;
        render->CreateSolidColorBrush(color, brush.GetAddressOf());
        colorBrush.push_back(std::move(brush));
    }
}

void WinToolSub::onPaint()
{
    render->Clear(D2D1::ColorF(0xFFFFFF,0.f));
    render->FillGeometry(borderPath.Get(), brushBg.Get());
    render->DrawGeometry(borderPath.Get(), brushSpliter.Get(), dpi);
    auto win = WinToolMain::get();
    if (win->state == "rect") {
        paintIcon(btnStart, getBtnIconLayout("rectFill"), hoverIndex == 0, selectIndex == 0);	
        paintSlider();
        paintColorSelector();
    }
    else if (win->state == "ellipse") {
        paintIcon(btnStart, getBtnIconLayout("ellipseFill"), hoverIndex == 0, selectIndex == 0);
        paintSlider();
        paintColorSelector();
    }
    else if (win->state == "arrow") {
        paintIcon(btnStart, getBtnIconLayout("arrowFill"), hoverIndex == 0, selectIndex == 0);
        paintSlider();
        paintColorSelector();
    }
    else if (win->state == "number") {
        paintIcon(btnStart, getBtnIconLayout("numberFill"), hoverIndex == 0, selectIndex == 0);
        paintColorSelector();
    }
    else if (win->state == "line") {
        paintIcon(btnStart, getBtnIconLayout("lineTransparent"), hoverIndex == 0, selectIndex == 0);
        paintSlider();
        paintColorSelector();
    }
    else if (win->state == "text") {
        paintIcon(btnStart, getBtnIconLayout("bold"), hoverIndex == 0, selectIndex == 0);
        paintIcon(btnStart+btnSize, getBtnIconLayout("italic"), hoverIndex == 1, selectIndex == 1);
        paintSlider();
        paintColorSelector();
    }
    else if (win->state == "mosaic") {
        paintIcon(btnStart, getBtnIconLayout("rectFill"), hoverIndex == 0, selectIndex == 0);
        paintSlider();
    }
    else if (win->state == "eraser") {
        paintIcon(btnStart, getBtnIconLayout("rectFill"), hoverIndex == 0, selectIndex == 0);
        paintSlider();
    }
}

bool WinToolSub::onCursor()
{
    SetCursor(LoadCursor(NULL, IDC_HAND));
    return TRUE;
}
void WinToolSub::onMouseMove(const int& x, const int& y)
{
    if (x > sliderStart && x < sliderEnd) {
        float val = ((float)x - sliderStart) / (sliderEnd - sliderStart) * (sliderMax - sliderMin)+sliderMin;
        tipText = std::format(L"{}", std::round(val));;
        showTipAt(this->x + x, this->y + marginTop + 4 * dpi);
    }
    bool needRefreshBtn = hoverBtn(x);
    bool needRefreshColor = hoverColor(x);
    if (needRefreshBtn|| needRefreshColor) {
        refresh();
    }
}
void WinToolSub::onMouseLeave()
{
    auto flag{ false };
    if (hoverColorIndex != -1) {
        hoverColorIndex = -1;
        flag = true;
    }
    if (hoverIndex != -1) {
        hoverIndex = -1;
        flag = true;
    }
    if (flag) refresh();
    hideTip();
}

void WinToolSub::onMouseDown(const int& x, const int& y, bool isRight)
{
    int indexBtn{ -1 };
    bool flag{ false };
    if (x > btnStart && x < btnEnd) {
        indexBtn = static_cast<int>((x - btnStart) / btnSize);
        if (indexBtn == hoverIndex)
        {
            if (indexBtn == selectIndex) {
                selectIndex = -1;
            }
            else {
                selectIndex = indexBtn;
            }
            flag = true;
        }
    }
    if (x > sliderStart && x < sliderEnd) {
        sliderVal = ((float)x - sliderStart) / (sliderEnd - sliderStart) * (sliderMax - sliderMin) + sliderMin;
        flag = true;
    }
    if (x > colorStart && x<colorEnd) {
        if (selectColorIndex != hoverColorIndex) {
            selectColorIndex = hoverColorIndex;
            flag = true;
        }
    }
    if (flag) refresh();

}

void WinToolSub::onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers)
{
    if (x < sliderStart || x > sliderEnd) return;
    sliderVal = ((float)x - sliderStart) / (sliderEnd - sliderStart) * (sliderMax - sliderMin) + sliderMin;
    tipText = std::format(L"{}", std::round(sliderVal));
    showTipAt(this->x + x, this->y + marginTop + 4 * dpi);
    refresh();
}

void WinToolSub::onMouseWheel(const int& x, const int& y, const short& delta)
{
    if (x > sliderEnd || x < sliderStart) return;
    if (delta < 0) {
        if (sliderVal < sliderMin) {
            sliderVal = sliderMin;
        }
        else if(sliderVal > sliderMin) {
            sliderVal -= 1.f;
        }
        else {
            return;
        }
    }
    else
    {
        if (sliderVal > sliderMax) {
            sliderVal = sliderMax;
        }
        else if(sliderVal < sliderMax) {
            sliderVal += 1.f;
        }
        else {
            return;
        }
    }
    refresh();
}

void WinToolSub::onKeyDown(const TCHAR& key)
{
    if (key == 'Z' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        parent->history->undo();
    }
    else if (key == 'Y' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        parent->history->redo();
    }
}

void WinToolSub::paintSlider()
{
    auto y{ marginTop + btnSize / 2 };
	render->DrawLine({ sliderStart,y }, { sliderStart +120.f, y }, brushSpliter.Get(), dpi);
    auto r{ 4.f * dpi };
    auto x{ sliderStart + (sliderEnd - sliderStart) * ((sliderVal-sliderMin) / (sliderMax - sliderMin)) };
	render->FillEllipse({ {x,y}, r, r }, brushSpliter.Get());
}

void WinToolSub::paintColorSelector()
{
    int index{ 0 };
    auto start{ colorStart };
    for (auto& brush:colorBrush)
    {
        D2D1_POINT_2F origin = { start, marginTop };
        auto icon = index == selectColorIndex? getBtnIconLayout("check") : getBtnIconLayout("uncheck");
        if (index == hoverColorIndex || index == selectColorIndex) {
            float paddingTopBottom{ 6.f * dpi }, paddingLeftRight{ 5.f * dpi };
            D2D1_ROUNDED_RECT rr = { { start + paddingLeftRight, paddingTopBottom + marginTop, start + btnSize - paddingLeftRight, h - paddingTopBottom }, 8, 8 };
            render->FillRoundedRectangle(rr, brushSelect.Get());
        }
        render->DrawTextLayout(origin, icon, brush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
        start += colorBtnW;
        index += 1;
    }
}


