#include "MainWin.h"
#include <Windows.h>
#include "WindowBase.h"
#include <dwmapi.h>


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
    Font::Init();
    auto win = new MainWin();
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    delete win;
    Font::Dispose();
    auto code = MainWin::GetQuitCode();
    return code;
}