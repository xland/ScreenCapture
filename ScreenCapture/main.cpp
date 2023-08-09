#include "MainWin.h"
#include <Windows.h>
#include <iostream>


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
    MainWin::Init(hInstance);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    MainWin::Dispose();
    auto code = MainWin::GetQuitCode();
    return code;
}
