#include "pch.h"
#include "Util.h"
#include "WinToolBase.h"
#include "WinToolSub.h"
#include "WinToolMain.h"

std::unique_ptr<WinToolSub> winToolSub;

WinToolSub::WinToolSub(const int& x, const int& y, const int& w, const int& h) :WinToolBase(x, y, w, h)
{
}

WinToolSub::~WinToolSub()
{

}

void WinToolSub::popup()
{
    if (!winToolSub.get()) {
        auto toolMain = WinToolMain::get();
        auto marginTop{ 5.f * toolMain->dpi };
        int x, y{toolMain->y+toolMain->h}, w, h{ int(toolMain->h+marginTop)};
        float arrowX;
		getXW(x, w,arrowX);
        winToolSub = std::make_unique<WinToolSub>(x, y, w, h);
        winToolSub->arrowX = arrowX;
        winToolSub->marginTop = marginTop;
		winToolSub->btnSize = (float)h - marginTop;
        winToolSub->createWindow(WS_EX_TOPMOST);
        winToolSub->initBrush();
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
        winToolSub->buildBorderPath();
        winToolSub->show();
    }
    else {
        auto toolMain = WinToolMain::get();
        auto win = winToolSub.get();
        win->marginTop = 5.f * toolMain->dpi;
        int x, y{(int)(toolMain->y + toolMain->h + 3.f * win->dpi) };
        getXW(x, win->w, win->arrowX);
        win->move(x, y);
        win->buildBorderPath();
        win->show();
    }
}
WinToolSub* WinToolSub::get()
{
    return winToolSub.get();
}
void WinToolSub::getXW(int& x, int& w, float& arrowX)
{
    auto toolMain = WinToolMain::get();
    if (toolMain->state == L"rect" || toolMain->state == L"ellipse" || toolMain->state == L"arrow" || toolMain->state == L"line") {
        w = 333 * toolMain->dpi;
    }
    else if (toolMain->state == L"number") {
        w = 250 * toolMain->dpi;
    }
    else if (toolMain->state == L"text") {
        w = 366 * toolMain->dpi;
    }
    else if (toolMain->state == L"mosaic"|| toolMain->state == L"eraser") {
        w = 126 * toolMain->dpi;
    }
    //计算主工具条选中按钮的中心位置
	arrowX = toolMain->h * toolMain->selectIndex + toolMain->h / 2; 
    if (arrowX+6.f*toolMain->dpi > w )
    {
		x = toolMain->x + arrowX - w /2; // 箭头顶点位于子工具条正中间
		arrowX = w / 2.f; 
    }
    else
    {
        x = toolMain->x; // 子工具条左侧与主工具条左侧对齐
    }

}
void WinToolSub::changeState()
{
	auto toolMain = WinToolMain::get();
    if (toolMain->state == L"") {
		hide();
    }
    else {
        int x, w;
        float arrowX;
        getXW(x, w, arrowX);
        if (w != this->w) {
            resize(w, this->h);
        }
        if (x != this->x) {
            move(x, this->y);
        }
		this->arrowX = arrowX;
        buildBorderPath();
        refresh();
    }
}

void WinToolSub::buildBorderPath()
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
        D2D1_POINT_2F{arrowX,2.f * dpi},
        D2D1_POINT_2F{arrowX + 3.f * dpi,marginTop},
        D2D1_POINT_2F{(float)w - borderW,marginTop},
        D2D1_POINT_2F{(float)w - borderW,(float)h - borderW},
        D2D1_POINT_2F{borderW,(float)h - borderW}
    };
    sink->AddLines(points, 6);
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
}

void WinToolSub::onPaint()
{
    render->Clear(D2D1::ColorF(0xFFFFFF,0.f));
    render->FillGeometry(borderPath.Get(), brushBg.Get());
    render->DrawGeometry(borderPath.Get(), brushSpliter.Get(), dpi);
    auto win = WinToolMain::get();
    posX = 0.f;
    auto span{ 8.f * dpi };
    if (win->state == L"rect") {
        paintIcon(posX, getBtnIconLayout(L"rectFill"), hoverIndex == 0, selectIndex == 0);
		posX += (btnSize + span);
        paintSlider();
        posX += (120.f + span);
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
    int index{-1};
    if (x < btnSize) index = 0;
    if (index != hoverIndex) {
        hoverIndex = index;
        refresh();
    }
}
void WinToolSub::onMouseLeave()
{
    hoverIndex = -1;
    InvalidateRect(hwnd, nullptr, false);
}

void WinToolSub::onMouseDown(const int& x, const int& y, bool isRight)
{
    int index{ -1 };
    if (x < btnSize) index = 0;
    if (index == hoverIndex)
    {
        if (index == selectIndex) {
            selectIndex = -1;
        }
        else {
            selectIndex = index;
        }
        refresh();
    }
}

void WinToolSub::paintSlider()
{
    auto y{ marginTop + btnSize / 2 };
	render->DrawLine({ posX,y }, { posX+120.f, y }, brushSpliter.Get(), dpi);
	D2D1_ELLIPSE circle = { {posX,y}, 4.f * dpi, 4.f * dpi };
	render->FillEllipse(circle, brushSpliter.Get());
}

void WinToolSub::paintColorSelector()
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
    for (auto& color:colors)
    {
        D2D1_POINT_2F origin = { posX, marginTop };
        auto icon = getBtnIconLayout(L"uncheck");
        ComPtr<ID2D1SolidColorBrush> brush;
        render->CreateSolidColorBrush(color, brush.GetAddressOf());
        render->DrawTextLayout(origin, icon, brush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
        posX += btnSize/3*2;
    }
}
