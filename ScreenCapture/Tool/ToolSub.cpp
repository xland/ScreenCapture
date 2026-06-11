#include "pch.h"
#include "Util.h"
#include "App.h"
#include "ToolBase.h"
#include "ToolSub.h"
#include "ToolMain.h"
#include "Win/WinPin.h"
#include "History.h"
#include "Shape/ShapeTextWin.h"
#include "ToolSubSlider.h"
#include "ToolSubColor.h"

ToolSub::ToolSub(WinPin* parent) :ToolBase(-999999, -999999, 1, 1,parent)
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
    if (toolMain->state == "rect" || toolMain->state == "ellipse") {
        auto isFillMode{ selectIndex == 0 };
        w = int((isFillMode ? 316 * toolMain->dpi - 120.f - span : 316 * toolMain->dpi) + 0.5f);
        slider->setVals(16.f, 1.f, 3.f, btnEnd + span, !isFillMode);
        colorer->setVals(isFillMode ? btnEnd : slider->end + span, true);
    }
    else if (toolMain->state == "arrow") {
        w = int(316 * toolMain->dpi);
        slider->setVals(60.f, 8.f, 18.f, btnEnd + span, true);
        colorer->setVals(slider->end + span, true);
        selectIndex = 0;
    }
    else if (toolMain->state == "line") {
        w = (int)(316 * toolMain->dpi);
        slider->setVals(60.f, 1.f, 3.f, btnEnd + span, true);
        colorer->setVals(slider->end + span, true);
    }
    else if (toolMain->state == "number") {
        w = (int)(228 * toolMain->dpi);
        selectIndex = 0;
        slider->setVals(0.f, 0.f, 0.f, 0.f, false);
        colorer->setVals(btnEnd, true);
    }
    else if (toolMain->state == "text") {
        w = (int)(348 * toolMain->dpi);
        btnEnd = btnStart + btnSize *2;
        slider->setVals(66.f, 16.f, 26.f, btnEnd + span, true);
        colorer->setVals(slider->end + span, true);
    }
    else if (toolMain->state == "mosaic" || toolMain->state == "eraser") {
        auto isRectMode{ selectIndex == 0 };
        w = int((isRectMode ? btnEnd : 126 * toolMain->dpi) + 0.5f);
        slider->setVals(86.f, 26.f, 32.f, btnEnd + span, !isRectMode);
        colorer->setVals(0, false);
    }
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
    btnId = { "rectFill" , "ellipseFill", "arrowFill", "numberFill" , "lineTransparent" ,"bold" , "italic", "check", "uncheck" };
    btnName = { L"矩形填充",L"圆形填充",L"箭头填充",L"标号填充",L"半透明",L"粗体",L"斜体" };
    addIconLayout(L"\ue602");
    addIconLayout(L"\ue600");
    addIconLayout(L"\ue604");
    addIconLayout(L"\ue605");
    addIconLayout(L"\ue607");
    addIconLayout(L"\ue634");
    addIconLayout(L"\ue682");
    addIconLayout(L"\ue721");
    addIconLayout(L"\ue61d");
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
    if (win->state == "rect") {
        paintIcon(btnStart, getBtnIconLayout("rectFill"), hoverIndex == 0, selectIndex == 0);
    }
    else if (win->state == "ellipse") {
        paintIcon(btnStart, getBtnIconLayout("ellipseFill"), hoverIndex == 0, selectIndex == 0);
    }
    else if (win->state == "arrow") {
        paintIcon(btnStart, getBtnIconLayout("arrowFill"), hoverIndex == 0, selectIndex == 0);
    }
    else if (win->state == "number") {
        paintIcon(btnStart, getBtnIconLayout("numberFill"), hoverIndex == 0, selectIndex == 0);
    }
    else if (win->state == "line") {
        paintIcon(btnStart, getBtnIconLayout("lineTransparent"), hoverIndex == 0, selectIndex == 0);
    }
    else if (win->state == "text") {
        paintIcon(btnStart, getBtnIconLayout("bold"), hoverIndex == 0, selectIndex == 0);
        paintIcon(btnStart + btnSize, getBtnIconLayout("italic"), hoverIndex == 1, selectIndex2 == 1);
    }
    else if (win->state == "mosaic" || win->state == "eraser") {
        paintIcon(btnStart, getBtnIconLayout("rectFill"), hoverIndex == 0, selectIndex == 0);
    }
}

void ToolSub::onCreated()
{
    btnSize = 32.f * dpi;
    marginTop = 4.f * dpi;
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
    if ((toolMain->state == "rect" || toolMain->state == "ellipse" || toolMain->state == "mosaic" || toolMain->state == "eraser") && oldSelectIndex != selectIndex) {
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

}


