#include <Windows.h>
#include "App.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr))
    {
        MessageBox(NULL, L"Failed to initialize COM library.", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }
    App::Init(std::wstring(lpCmdLine));
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    App::Dispose();
    CoUninitialize();
    auto code = App::GetExitCode();
    return code;
}