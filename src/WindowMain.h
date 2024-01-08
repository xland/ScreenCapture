#pragma once
#include <Windows.h>
#include <string>
#include <format>
#include "WindowBase.h"
#include <vector>
#include "include/core/SkRect.h"

class WindowMain : public WindowBase
{
public:
    WindowMain();
    ~WindowMain();
    void Save(const std::string& filePath) override;
    void SaveToClipboard() override;
private:
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;
    void initCanvas() override;
    void paintCanvas() override;
    void initSize() override;
    void shotScreen();
    bool onMouseDown(const int& x, const int& y);
    bool onMouseDownRight(const int& x, const int& y);
    bool onMouseUp(const int& x, const int& y);
    bool onMouseMove(const int& x, const int& y);
    bool onMouseDrag(const int& x, const int& y);
    bool onChar(const unsigned int& val);
    bool onKeyDown(const unsigned int& val);
    bool onMouseWheel(const int& delta);
};