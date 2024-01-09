#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "ToolBase.h"
#include "ToolBtn.h"
#include <vector>

class ToolMain : public ToolBase
{
public:
    ~ToolMain();
    static void Init();
    static ToolMain *Get();
    bool OnMouseDown(const int& x, const int& y) override;
    void SetPositionByCutMask();
    void SetPosition(const float& x,const float& y);
    bool OnPaint(SkCanvas *canvas) override;
    void SetUndoDisable(bool flag);
    void SetRedoDisable(bool flag);
    void InitBtns();
    void Reset();
private:
    ToolMain();
    void saveFile();
};