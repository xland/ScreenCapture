#include "pch.h"
#include "Util.h"
#include "App.h"
#include "Lang.h"
#include "ToolBase.h"
#include "ToolSub.h"
#include "ToolMain.h"
#include "Win/WinPin.h"
#include "History.h"
#include "Shape/ShapeTextWin.h"
#include "ToolSubSlider.h"
#include "ToolSubColor.h"

ToolSub::ToolSub(WinPin* parent, const int& x, const int& y, const int& w, const int& h) :ToolBase(x, y, w, h), parent{ parent }
{
    slider = std::make_unique<ToolSubSlider>(this);
    colorer = std::make_unique<ToolSubColor>(this);
}

ToolSub::~ToolSub()
{

}

void ToolSub::resetVal()
{
    auto toolMain = parent->toolMain.get();
    auto span{ 4.f * dpi };
    y = int(toolMain->y + toolMain->h + 3.f * dpi+0.5);
    h = int(toolMain->h + marginTop + 0.5);
    btnStart = 0.f;
    btnEnd = btnStart + btnSize;
    if (toolMain->state == L"rect" || toolMain->state == L"ellipse") {
        auto isFillMode{ selectIndex == 0 };
        // w = int((isFillMode ? 316 * toolMain->dpi - 120.f - span : 316 * toolMain->dpi) + 0.5f);
        slider->setVals(16.f, 1.f, 3.f, btnEnd + span, !isFillMode);
        colorer->setVals(isFillMode ? btnEnd : slider->end + span, true);
    }
    else if (toolMain->state == L"arrow") {
        slider->setVals(60.f, 8.f, 18.f, btnEnd + span, true);
        colorer->setVals(slider->end + span, true);
        selectIndex = 0;
    }
    else if (toolMain->state == L"line") {
        slider->setVals(60.f, 1.f, 3.f, btnEnd + span, true);
        colorer->setVals(slider->end + span, true);
    }
    else if (toolMain->state == L"number") {
        selectIndex = 0;
        slider->setVals(0.f, 0.f, 0.f, 0.f, false);
        colorer->setVals(btnEnd, true);
    }
    else if (toolMain->state == L"text") {
        btnEnd = btnStart + btnSize *2;
        slider->setVals(66.f, 16.f, 26.f, btnEnd + span, true);
        colorer->setVals(slider->end + span, true);
    }
    else if (toolMain->state == L"mosaic" || toolMain->state == L"eraser") {
        auto isRectMode{ selectIndex == 0 };
        w = int((isRectMode ? btnEnd : 126 * toolMain->dpi) + 0.5f);
        slider->setVals(86.f, 26.f, 32.f, btnEnd + span, !isRectMode);
        colorer->setVals(0, false);
    }
    w = int(std::max({ btnEnd, colorer->getRequiredWidth(), slider->getRequiredWidth() }) + 0.5f);
    colorer->end = (float)w;
    arrowX = btnSize * toolMain->selectIndex + btnSize / 2;
    if (arrowX + 6.f * toolMain->dpi > w)
    {
        x = int(toolMain->x + arrowX - w / 2+0.5); // 箭头顶点位于子工具条正中间
        arrowX = w / 2.f;
    }
    else
    {
        x = toolMain->x; // 子工具条左侧与主工具条左侧对齐
    }
    move(x, y);
    resize(w, h);
    setBorderPath();
}

void ToolSub::initBtn()
{
    btnLayout.clear();
    auto fSize{ 14.f * dpi };
    btnLayout.push_back(makeIconLayout(L"\ue602", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue600", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue604", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue605", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue607", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue634", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue682", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue6ad", btnSize, btnSize, 9.f*dpi)); //check
}

bool ToolSub::hoverBtn(const int& x)
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
            auto toolMain = parent->toolMain.get();
            if (toolMain->state == L"ellipse") startIndex = 1;
            if (toolMain->state == L"arrow")startIndex = 2;
            if (toolMain->state == L"number")startIndex = 3;
            if (toolMain->state == L"line")startIndex = 4;
            if (toolMain->state == L"text")startIndex = 5;
            tipText = Lang::get(std::format(L"tool.{}", btnId[startIndex + indexBtn]));
            showTipAt((int)(this->x + btnStart + indexBtn * btnSize + btnSize / 2), (int)(this->y + marginTop + 4 * dpi));
        }
        return true;
    }
    return false;
}

void ToolSub::setBorderPath()
{
    borderPath.Reset();
    App::getD2D()->CreatePathGeometry(borderPath.GetAddressOf());
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

void ToolSub::onPaint()
{
    render->Clear(0);
    render->FillGeometry(borderPath.Get(), brushBg.Get());
    render->DrawGeometry(borderPath.Get(), brushSpliter.Get(), dpi);
    paintToolButtons();
    slider->paint();
    colorer->paint();
}

void ToolSub::paintToolButtons()
{
    auto win = parent->toolMain.get();
    if (win->state == L"rect") {
        paintIcon(btnStart, getBtnIconLayout(L"rectFill"), hoverIndex == 0, selectIndex == 0);
    }
    else if (win->state == L"ellipse") {
        paintIcon(btnStart, getBtnIconLayout(L"ellipseFill"), hoverIndex == 0, selectIndex == 0);
    }
    else if (win->state == L"arrow") {
        paintIcon(btnStart, getBtnIconLayout(L"arrowFill"), hoverIndex == 0, selectIndex == 0);
    }
    else if (win->state == L"number") {
        paintIcon(btnStart, getBtnIconLayout(L"numberFill"), hoverIndex == 0, selectIndex == 0);
    }
    else if (win->state == L"line") {
        paintIcon(btnStart, getBtnIconLayout(L"semiTransparent"), hoverIndex == 0, selectIndex == 0);
    }
    else if (win->state == L"text") {
        paintIcon(btnStart, getBtnIconLayout(L"bold"), hoverIndex == 0, selectIndex == 0);
        paintIcon(btnStart + btnSize, getBtnIconLayout(L"italic"), hoverIndex == 1, selectIndex2 == 1);
    }
    else if (win->state == L"mosaic" || win->state == L"eraser") {
        paintIcon(btnStart, getBtnIconLayout(L"rectFill"), hoverIndex == 0, selectIndex == 0);
    }
}

void ToolSub::onCreated()
{
    btnSize = 32.f * dpi;
    marginTop = 4.f * dpi;
    btnId = { L"rectFill" , L"ellipseFill", L"arrowFill", L"numberFill" , L"semiTransparent" ,L"bold" , L"italic", L"check" };
    initTip();
    initBrush();
    initBtn();
    colorer->winReady();
}

BOOL ToolSub::onCursor()
{
    SetCursor(LoadCursor(NULL, IDC_HAND));
    return TRUE;
}
void ToolSub::onMouseMove(const int& x, const int& y)
{
    slider->mouseMove(x, y);
    bool needRefreshBtn = hoverBtn(x);
    bool needRefreshColor = colorer->hover(x);
    if (needRefreshBtn|| needRefreshColor) {
        refresh();
    }
}
void ToolSub::onMouseLeave()
{
    auto flag{ false };
    if (colorer->indexHovered != -1) {
        colorer->indexHovered = -1;
        flag = true;
    }
    if (hoverIndex != -1) {
        hoverIndex = -1;
        flag = true;
    }
    if (flag) refresh();
    hideTip();
}

void ToolSub::onMouseDown(const int& x, const int& y, bool isRight)
{
    bool flag{ false };
    auto toolMain = parent->toolMain.get();
    auto oldSelectIndex{ selectIndex };
    if (x > btnStart && x < btnEnd) {
        if (hoverIndex == 0 && selectIndex == 0) {
            selectIndex = -1;
            flag = true;
            ShapeTextWin::get()->changeState();
        }
        else if (hoverIndex == 1 && selectIndex2 == 1) {
            selectIndex2 = -1;
            flag = true;
            ShapeTextWin::get()->changeState();
        }
        else if (hoverIndex == 0) {
            selectIndex = hoverIndex;
            flag = true;
            ShapeTextWin::get()->changeState();
        }
        else if (hoverIndex == 1) {
            selectIndex2 = hoverIndex;
            flag = true;
            ShapeTextWin::get()->changeState();
        }
    }
    auto sliderRefresh = slider->mouseDown(x, y);
    auto colorRefresh = colorer->mouseDown(x, y);
    if ((toolMain->state == L"rect" || toolMain->state == L"ellipse" || toolMain->state == L"mosaic" || toolMain->state == L"eraser") && oldSelectIndex != selectIndex) {
        resetVal();
    }
    else if (flag || colorRefresh || sliderRefresh) {
        refresh();
    }

}

void ToolSub::onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers)
{
    slider->mouseDrag(x, y);
}

void ToolSub::onMouseWheel(const int& x, const int& y, const short& delta)
{

    slider->mouseWheel(x, y,delta);
}

void ToolSub::onDpiChanged()
{
    btnSize = 32.f * dpi;
    colorer->btnSize = 23 * dpi;
    initBtn();
}

void ToolSub::onKeyDown(const UINT& key)
{
    if (key == 'Z' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        parent->history->undo();
    }
    else if (key == 'Y' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        parent->history->redo();
    }
    else if (key == 'C' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        parent->copyToClipboard();
    }
    else if (key == 'S' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        parent->saveToFile();
    }
    else if (key == VK_RETURN)
    {
        parent->copyToClipboard();
    }
    else if (key == VK_DELETE)
    {
        parent->history->removeHoverShape();
    }
    else if (key == VK_ESCAPE)
    {
        parent->close();
    }
}