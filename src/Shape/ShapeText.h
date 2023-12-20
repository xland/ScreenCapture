#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "include/core/SkPath.h"
#include "ShapeRect.h"
#include <vector>

class ShapeText : public ShapeBase
{
public:
    ShapeText(const int &x, const int &y);
    ~ShapeText();
    bool OnMouseDown(const int &x, const int &y) override;
    bool OnMouseMove(const int &x, const int &y) override;
    bool OnMouseUp(const int &x, const int &y) override;
    bool OnMoseDrag(const int &x, const int &y) override;
    bool OnPaint(SkCanvas *canvas) override;
    void InsertWord(const std::wstring& word);

protected:
private:
    SkColor color{ SkColorSetARGB(255, 207, 19, 34) };
    void activeKeyboard(long x, long y);
    void initParams();
    SkRect rect;
    std::wstring text{ L"Line 1\nLine 2\nLine 3" };
    unsigned cursorIndex = 0;
    bool stroke{true};
    int strokeWidth{4};
};