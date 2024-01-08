#pragma once
#include <Windows.h>
#include <string>
#include <format>
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPictureRecorder.h"
#include "State.h"

#define WM_REFRESH (WM_APP+1)
class WindowBase
{
public:
    WindowBase();
    virtual ~WindowBase();
    void Show();
    void Refresh();
    void refresh();
    void Close(const int &exitCode);
    virtual void Save(const std::string& filePath) = 0;
    virtual void SaveToClipboard() = 0;
    HWND hwnd;
    State state = State::start;
    int x, y, w, h;
    bool IsMouseDown{false};
    sk_sp<SkSurface> surfaceBase;
    sk_sp<SkSurface> surfaceBack;
    sk_sp<SkSurface> surfaceFront;
    SkPixmap* pixSrc;
    SkPixmap* pixBase;

protected:
    virtual LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;
    virtual void initCanvas() = 0;
    virtual void paintCanvas() = 0;
    virtual void initSize() = 0;
    void initWindow();
    HDC hCompatibleDC = NULL;
    HBITMAP bottomHbitmap;

private:
    void setClipboardText(const std::wstring& text);
    std::wstring getPixelRgb();
    std::wstring getPixelHex();
    static LRESULT CALLBACK RouteWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};