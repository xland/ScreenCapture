#include "MainWin.h"
#include <Windows.h>
#include <iostream>


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
    AttachConsole(ATTACH_PARENT_PROCESS);
    DWORD dwCharsWritten;
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), L"allen", 6, &dwCharsWritten,NULL);
    MainWin::Init(hInstance);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    MainWin::Dispose();
    return 0;
}
