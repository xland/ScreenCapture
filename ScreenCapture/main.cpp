#include <Windows.h>
#include "HelloWorld.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow){
    auto app = new HelloWorld(hInstance);
    MSG msg = { };
    bool idled = false;

    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);

            if (WM_PAINT == msg.message) {
                // Ensure that call onIdle at least once per WM_PAINT, or mouse events can
                // overwhelm the message processing loop, and prevent animation from running.
                if (!idled) {
                    app->onIdle();
                }
                idled = false;
            }
            DispatchMessage(&msg);
        }
        else {
            app->onIdle();
            idled = true;
        }
    }
    delete app;
    return (int)msg.wParam;
}
