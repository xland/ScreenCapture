#include <QtWidgets/QApplication>
#include "WindowNative.h"
#include "WindowMain.h"


//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    WindowNative::Init();
//    return a.exec();
//}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
    //MainWin::Init(hInstance, std::wstring(lpCmdLine));
    int argc = __argc;
    char** argv = __argv;
    QApplication app(argc, argv);
    WindowNative::Init();
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        app.processEvents();
    }
    //MainWin::Dispose();
    return 0;
}
