#include "pch.h"
#include "../Win/WinPin.h"
#include "ShapeBase.h"

ShapeBase::ShapeBase(WinPin* win):win{win}, draggerSize{6*win->dpi}
{
	win->render->CreateSolidColorBrush(D2D1::ColorF(0x000000), brushDragger.GetAddressOf());
}

ShapeBase::~ShapeBase()
{}

ID2D1StrokeStyle* ShapeBase::getRoundStrokeStyle()
{
    static ComPtr<ID2D1StrokeStyle> roundStrokeStyle;
    if (!roundStrokeStyle.Get()) {
        win->getD2D()->CreateStrokeStyle(
            D2D1::StrokeStyleProperties(
                D2D1_CAP_STYLE_ROUND,    // 起点线帽：圆角
                D2D1_CAP_STYLE_ROUND,    // 终点线帽：圆角
                D2D1_CAP_STYLE_ROUND,    // 虚线端点（如有）
                D2D1_LINE_JOIN_ROUND,    // 线段连接处：圆角
                8.f,                     // miterLimit
                D2D1_DASH_STYLE_SOLID,
                0.0f
            ),
            nullptr,
            0,
            roundStrokeStyle.GetAddressOf()
        );
    }
    return roundStrokeStyle.Get();
}