#pragma once
#include <Windows.h>
#include <string>
#include <format>
#include <vector>
#include <include/core/SkSurface.h>
#include <include/core/SkCanvas.h>

#include "../State.h"

class WinBase
{
public:
    WinBase();
    virtual ~WinBase();
    void Show();
    void Refresh();
    void SetTimeout(const unsigned int& id,const unsigned int& ms);
    void ClearTimeout(const unsigned int& id);
    void Close(const int &exitCode);
    virtual void Save(const std::string& filePath) = 0;
    virtual void SaveToClipboard() = 0;
public:
    HWND hwnd;
    HWND hwndTip;
    State state = State::start;
    int x, y, w, h;
    bool isMouseDown{false};

protected:
    virtual LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;
    virtual void initCanvas() = 0;
    virtual void paint(SkCanvas* canvas) = 0;
    void initWindow();
    std::vector<SkColor> pix;
    std::unique_ptr<SkCanvas> canvas;
private:
    void refresh();
    std::wstring getPixelRgb();
    std::wstring getPixelHex();
    static LRESULT CALLBACK RouteWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};