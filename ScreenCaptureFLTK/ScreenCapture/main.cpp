#include <Windows.h>
#include <FL/Fl.H>
#include "MainWindow.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	MainWindow::Init(hInstance);
	return Fl::run();
	//MSG msg;
	//while (GetMessage(&msg, NULL, 0, 0) > 0)
	//{
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//}
	//return (int)msg.wParam;
}