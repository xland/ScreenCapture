#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <string>
#include <memory>
#include <vector>
#include <FL/Fl_Double_Window.H>
#include <FL/x.h>
#include <FL/fl_draw.H>
#include <FL/Fl_Bitmap.H>
#include <FL/Fl_Tiled_Image.H>
class MainWindow:Fl_Double_Window
{
public:
	MainWindow(HINSTANCE hinstance, int x, int y, int w, int h);
	~MainWindow();
protected:
	//void draw() override;
private:
	void shotVirtualScreen();
	void initWindowGeomtry();
	Fl_Bitmap* bgImg;
	HINSTANCE hinstance;
	HWND hwnd;
	int x, y, w, h;	
	std::uint8_t* pixels;
};
namespace {
	std::shared_ptr<MainWindow> mainWindow;
}

