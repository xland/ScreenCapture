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
        winToolSub->createWindow(WS_EX_TOPMOST);
        winToolSub->initBrush();
        winToolSub->enableAlpha();
        winToolSub->addBtns({ {L"rectFill" ,L"\ue602"},
        {L"ellipseFill" ,L"\ue600"},
        {L"arrowFill" ,L"\ue604"},
        {L"numberFill" ,L"\ue605"},
        {L"lineTransparent" ,L"\ue607"},
        {L"bold" ,L"\ue634"},
        {L"italic" ,L"\ue682"}});
        winToolSub->show();
    }
    else {
        auto toolMain = WinToolMain::get();
        auto win = winToolSub.get();
        win->marginTop = 5.f * toolMain->dpi;
        int x, y{(int)(toolMain->y + toolMain->h + 3.f * win->dpi) };
        getXW(x, win->w, win->arrowX);
        win->move(x, y);
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
        w = 222 * toolMain->dpi;
    }
    else if (toolMain->state == L"number") {
        w = 166 * toolMain->dpi;
    }
    else if (toolMain->state == L"text") {
        w = 244 * toolMain->dpi;
    }
    else if (toolMain->state == L"mosaic"|| toolMain->state == L"eraser") {
        w = 84 * toolMain->dpi;
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
        if (x != this->x) {
            move(x, this->y);
        }
        if (w != this->w) {
            resize(w, this->h);
        }
		this->arrowX = arrowX;
        refresh();
    }
}

void WinToolSub::paintBorder()
{
    ComPtr<ID2D1PathGeometry> path;
	auto d2d = Util::getD2D();
    d2d->CreatePathGeometry(path.GetAddressOf());
    ComPtr<ID2D1GeometrySink> sink;
    path->Open(sink.GetAddressOf());
    auto borderW{dpi};
    sink->BeginFigure({ borderW,marginTop }, D2D1_FIGURE_BEGIN_FILLED);
    std::vector<D2D1_POINT_2F> points = { 
        D2D1_POINT_2F{arrowX - 3.f * dpi,marginTop},
        D2D1_POINT_2F{arrowX,2.f*dpi},
        D2D1_POINT_2F{arrowX + 3.f * dpi,marginTop},
        D2D1_POINT_2F{(float)w- borderW,marginTop},
        D2D1_POINT_2F{(float)w- borderW,(float)h- borderW},
        D2D1_POINT_2F{borderW,(float)h- borderW}
    };
    sink->AddLines(points.data(), 6);
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    render->FillGeometry(path.Get(), brushBg.Get());
    render->DrawGeometry(path.Get(), brushSpliter.Get(), borderW);
}

void WinToolSub::onPaint()
{
    render->Clear(D2D1::ColorF(0xFFFFFF,0.f));
    paintBorder();

    //int btnIndex{ 0 };
    //for (auto& pair : btnIcons)
    //{
    //    if (btnIndex == hoverIndex || pair.first == state) {
    //        auto xStart{ btnW * btnIndex };
    //        D2D1_ROUNDED_RECT rr = { { xStart + btnPadding, btnPadding, xStart + btnW - btnPadding, h - btnPadding }, 8, 8 };
    //        render->FillRoundedRectangle(rr, brushBg.Get());
    //        D2D1_POINT_2F origin = { xStart, 0.f };
    //        render->DrawTextLayout(origin, pair.second.Get(), brushBlue.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
    //    }
    //    else {
    //        D2D1_POINT_2F origin = { btnW * btnIndex, 0.f };
    //        render->DrawTextLayout(origin, pair.second.Get(), brushIcon.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
    //    }
    //    btnIndex += 1;
    //}
}

bool WinToolSub::onCursor()
{
    SetCursor(LoadCursor(NULL, IDC_HAND));
    return TRUE;
}