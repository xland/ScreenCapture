#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "ToolBase.h"

class ToolMain : public ToolBase
{
public:
    ToolMain();
    ~ToolMain();
    void Init() override;
    void OnLeftBtnDown(const int& x, const int& y) override;
    void OnCustomMsg(const EventType& type, const uint32_t& msg);
    void OnPaint(SkCanvas* canvas) override;
    void SetPosition(const float& x, const float& y);
private:
    void setPositionByCutMask();
    /// <summary>
    /// 0主工具条在选中或取消选中某个按钮时不需要改变位置
    /// 1选中某个按钮时，位置上移
    /// 2取消选中某个按钮时，位置下移
    /// </summary>
    bool topFlag{ false };
};