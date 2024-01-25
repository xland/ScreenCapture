#pragma once
#include <Windows.h>
#include <string>
#include <format>
#include <vector>
#include "include/core/SkSurface.h"
#include "State.h"

#define WM_REFRESH (WM_APP+1)
#define WM_SHOW_DRAGGER (WM_APP+2)
#define WM_HIDE_DRAGGER (WM_APP+3)
#define WM_FLASH_CURSOR (WM_APP+4)

class WindowBase
{
public:
    WindowBase();
    virtual ~WindowBase();
    void Show();
    void Refresh();
    void refresh();
    void SetTimeout(const unsigned int& id,const unsigned int& ms);
    void ClearTimeout(const unsigned int& id);
    void Close(const int &exitCode);
    virtual void Save(const std::string& filePath) = 0;
    virtual void SaveToClipboard() = 0;
    HWND hwnd;
    HWND hwndToolTip;
    State state = State::start;
    int x, y, w, h;
    bool IsMouseDown{false};
    sk_sp<SkSurface> surfaceBase;
    sk_sp<SkSurface> surfaceBack;
    sk_sp<SkSurface> surfaceFront;
    std::vector<int32_t> pixSrcData;
    SkPixmap* pixSrc{ nullptr };
    SkPixmap* pixBase{nullptr};
    SkPixmap* pixBack{ nullptr };

protected:
    virtual LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;
    virtual void initCanvas() = 0;
    virtual void paintCanvas() = 0;
    void initWindow();
    HDC hCompatibleDC = NULL;
    HBITMAP bottomHbitmap;

private:
    void setClipboardText(const std::wstring& text);
    std::wstring getPixelRgb();
    std::wstring getPixelHex();
    static LRESULT CALLBACK RouteWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};