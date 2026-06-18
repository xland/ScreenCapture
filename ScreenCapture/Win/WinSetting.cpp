#include "pch.h"
#include "WinSetting.h"
#include "Util.h"

std::unique_ptr<WinSetting> winSetting;

WinSetting::WinSetting(const int& x, const int& y, const int& w, const int& h) :WinBase(x, y, w, h)
{
	
}

WinSetting::~WinSetting()
{

}

void WinSetting::init()
{
	RECT workAreaRect;
	BOOL getWorkAreaSuccess = SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0); //工作区矩形
	int workAreaWidth = workAreaRect.right - workAreaRect.left;
	int workAreaHeight = workAreaRect.bottom - workAreaRect.top;
	int w = 800;
	int h = 600;
	int x = workAreaRect.left + (workAreaWidth - w) / 2;
	int y = workAreaRect.top + (workAreaHeight - h) / 2;
	winSetting = std::make_unique<WinSetting>(x, y, w, h);
	winSetting->createWindow(WS_EX_APPWINDOW, WS_MAXIMIZEBOX | WS_MINIMIZEBOX);
}

void WinSetting::onCreated()
{
	SetWindowText(hwnd, L"ScreenCapture设置");
	enableShadow();
	render->CreateSolidColorBrush(D2D1::ColorF(0xEEEEF0), menuBg.GetAddressOf());
	render->CreateSolidColorBrush(D2D1::ColorF(0xDADADC), border.GetAddressOf());
	show();
}

void WinSetting::onPaint()
{
	render->Clear(D2D1::ColorF(0xE3E3E5));
	D2D1_RECT_F menuRect{ 0,0,200,h };
	render->FillRectangle(menuRect, menuBg.Get());
	render->DrawLine({ 200,0 }, { 200,(float)h }, border.Get(), dpi);
}

BOOL WinSetting::onCursor()
{
	setCursor(IDC_ARROW);
	return TRUE;
}
