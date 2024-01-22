#pragma once
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "ShapeBase.h"
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
    bool OnChar(const unsigned int& val) override;
    bool OnKeyDown(const unsigned int& val) override;
    void Paint(SkCanvas *canvas) override;   
    bool OnMouseWheel(const int& delta) override;
    void OnShowDragger(SkCanvas* canvas) override;
    bool EndInput();
protected:
private:
    SkColor color{ SkColorSetARGB(255, 207, 19, 34) };
    void activeKeyboard(long x, long y);
    void setRect(SkCanvas* canvas);
    void setCursor(SkCanvas* canvas);
    float getCursorX();
    int hoverX, hoverY;
    SkRect rect;
    std::vector<std::wstring> lines;
    float lineHeight;
    int lineIndex{ 0 }, wordIndex{0};
    float fontSize{ 60 };
    bool isMouseOver{true};
    bool showCursor{ true };
};