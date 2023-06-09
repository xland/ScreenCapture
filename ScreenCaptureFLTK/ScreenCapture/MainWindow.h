#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <string>
#include <memory>
#include <vector>
#include <FL/Fl_Double_Window.H>
#include <FL/x.h>
#include <FL/fl_draw.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_Tiled_Image.H>
class MainWindow:Fl_Double_Window
{
public:
	MainWindow(HINSTANCE hinstance, int x, int y, int w, int h);
	~MainWindow();
	static void Init(HINSTANCE hinstance);
	LRESULT CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
protected:
	void draw() override;
private:
	void createWin32Window();
	void shotVirtualScreen();
	void initWindowGeomtry();

	HINSTANCE hinstance;
	HWND hwnd;
	std::shared_ptr<Fl_Window> win;
	Fl_RGB_Image* bgImg;
	Fl_Tiled_Image* bgImg2;
	int x, y, w, h;
	std::vector<std::uint8_t> pixels;
};
namespace {
	std::shared_ptr<MainWindow> mainWindow;
}

