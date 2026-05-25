#include "pch.h"
#include "Util.h"
#include "WinToolBase.h"
#include "WinToolSub.h"
#include "WinToolMain.h"

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
    auto span{ 8.f * toolMain->dpi };
    y = toolMain->y + toolMain->h + 3.f * toolMain->dpi;
    h = int(toolMain->h + marginTop);
    btnSize = toolMain->h;
    colorBtnW = toolMain->dpi * 23;
    if (toolMain->state == L"rect" || toolMain->state == L"ellipse" || toolMain->state == L"arrow" || toolMain->state == L"line") {
        w = 319 * toolMain->dpi;
        sliderMax = 22;
        sliderMin = 12;
        sliderVal = 16;
        btnStart = 0;
        btnEnd = btnStart + btnSize;
        sliderStart = btnEnd + span;
        sliderEnd = sliderStart + 120.f;
        colorStart = sliderEnd + span;
        colorEnd = w;
    }
    else if (toolMain->state == L"number") {
        w = 250 * toolMain->dpi;
    }
    else if (toolMain->state == L"text") {
        w = 366 * toolMain->dpi;
    }
    else if (toolMain->state == L"mosaic" || toolMain->state == L"eraser") {
        w = 126 * toolMain->dpi;
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

void WinToolSub::popup()
{
    if (!winToolSub.get()) {
        winToolSub = std::make_unique<WinToolSub>();
        winToolSub->initVal();
        winToolSub->createWindow(WS_EX_TOPMOST);
        winToolSub->initToolTip();
        winToolSub->initBrush();
        winToolSub->initColor();
        winToolSub->enableAlpha();
        winToolSub->addBtns({ {L"rectFill" ,L"\ue602"},
        {L"ellipseFill" ,L"\ue600"},
        {L"arrowFill" ,L"\ue604"},
        {L"numberFill" ,L"\ue605"},
        {L"lineTransparent" ,L"\ue607"},
        {L"bold" ,L"\ue634"},
        {L"italic" ,L"\ue682"},
        {L"check" ,L"\ue721"},
        {L"uncheck" ,L"\ue61d"} });
        winToolSub->initBorder();
        winToolSub->show();
    }
    else {
        auto toolMain = WinToolMain::get();
        auto win = winToolSub.get();
        win->move(win->x, win->y);
        win->initBorder();
        win->show();
    }
}
WinToolSub* WinToolSub::get()
{
    return winToolSub.get();
}
void WinToolSub::changeState()
{
	auto toolMain = WinToolMain::get();
    if (toolMain->state == L"") {
		hide();
    }
    else {
        initVal();
        resize(w, h);
        move(x, y);
        initBorder();
        refresh();
    }
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
    if (win->state == L"rect") {
        paintIcon(btnStart, getBtnIconLayout(L"rectFill"), hoverIndex == 0, selectIndex == 0);	
        paintSlider();
        paintColorSelector();
    }
}

bool WinToolSub::onCursor()
{
    SetCursor(LoadCursor(NULL, IDC_HAND));
    return TRUE;
}
void WinToolSub::onMouseMove(const int& x, const int& y)
{
    int indexBtn{ -1 }, indexColor{-1};
    bool flag{ false };
    if (x > btnStart && x < btnEnd) {
        indexBtn = static_cast<int>((x - btnStart) / btnSize);
        if (indexBtn != hoverIndex) {
            hoverIndex = indexBtn;
            showToolTipAt(btnStart + hoverIndex * btnSize + btnSize / 2, 6, L"测试1");
            flag = true;
        }
    }
    if (x > colorStart && x < colorEnd) {
        auto indexColor = static_cast<int>((x- colorStart) / colorBtnW);
        if (indexColor != hoverColorIndex) {
            hoverColorIndex = indexColor;
            static std::wstring text{ L"测试" };
            showToolTipAt(colorStart + hoverColorIndex * colorBtnW+ colorBtnW/2, 6, text.data());
            flag = true;
        }
    }
    if (flag) {
        refresh();
    }
}
void WinToolSub::onMouseLeave()
{
    hoverIndex = -1;
    hoverColorIndex = -1;
    InvalidateRect(hwnd, nullptr, false);
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
    if (x > colorStart && x<colorEnd) {
        if (selectColorIndex != hoverColorIndex) {
            selectColorIndex = hoverColorIndex;
            flag = true;
        }
    }
    if (flag) refresh();

}

void WinToolSub::onMouseWheel(const int& x, const int& y, const short& delta)
{
    if (x > sliderEnd || x < sliderStart) return;
    if (delta < 0) {
        if (sliderVal < sliderMin) {
            sliderVal = sliderMin;
        }
        else if(sliderVal > sliderMin) {
            sliderVal -= 1;
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
            sliderVal += 1;
        }
        else {
            return;
        }
    }
    refresh();
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
        auto icon = index == selectColorIndex? getBtnIconLayout(L"check") : getBtnIconLayout(L"uncheck");
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


