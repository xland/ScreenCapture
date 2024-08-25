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
    static void Init();
    static ToolMain* Get();
    bool OnMouseDown(const int& x, const int& y) override;
    void SetPositionByCutMask();
    void SetPosition(const float& x, const float& y);
    bool Paint(SkCanvas* canvas) override;
    void SetUndoDisable(bool flag);
    void SetRedoDisable(bool flag);
    void InitBtns();
    void UnSelectAndHoverAll();
private:
    /// <summary>
    /// 0主工具条在选中或取消选中某个按钮时不需要改变位置
    /// 1选中某个按钮时，位置上移
    /// 2取消选中某个按钮时，位置下移
    /// </summary>
    bool topFlag{ false };
};