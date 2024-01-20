#pragma once
#include <Windows.h>
#include <string>
#include <format>
#include "WindowBase.h"
#include "State.h"
#include "include/core/SkImage.h"
#include "include/core/SkPixmap.h"

class WindowPin : public WindowBase
{
public:
    WindowPin();
    ~WindowPin();
    void Save(const std::string& filePath) override;
    void SaveToClipboard() override;
private:
    void initCanvas() override;
    void initSize() override;
    void showMenu();
    LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;
    void paintCanvas() override;
    bool onMouseDown(const int& x, const int& y);
    bool onMouseDownRight(const int& x, const int& y);
    bool onMouseUp(const int& x, const int& y);
    bool onMouseMove(const int& x, const int& y);
    bool onMouseDrag(const int& x, const int& y);
    bool onChar(const unsigned int& val);
    bool onKeyDown(const unsigned int& val);
    bool onMouseWheel(const int& delta);
    bool onTimeout(const unsigned int& id);
    POINT startPos;
    float shadowSize{ 8.0f };
    int hoverIndex{ -1 };
    sk_sp<SkImage> img;
    SkRect imgRect;
    float imgW, imgH;
};