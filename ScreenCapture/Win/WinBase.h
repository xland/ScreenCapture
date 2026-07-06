#pragma once
class WinBase
{
	friend class App;
public:
	WinBase(const int& x, const int& y, const int& w, const int& h);
	virtual ~WinBase();
	void show();
	void hide();
	void refresh();
	void close();
	void move(const int& x, const int& y);
	void resize(const int& w, const int& h);
	void createWindow(const DWORD& exStyle=NULL, const DWORD& style=NULL,const std::wstring& title = L"");
	HRESULT createBitmap();
	void enableShadow();
	void setTimer(const UINT& elapse, const UINT& id);
	void killTimer(const UINT& id);
	void setCursor(LPCWSTR cursorName);
	/// 重建本窗口的渲染目标（D3D 设备丢失/重置时调用）。
	/// 释放 swap chain / render / bitmap，再基于（可能已重建的）全局设备重新 makeDC。
	void handleDeviceLost();
public:
	HWND hwnd{nullptr};
	int x, y, w, h;
	float dpi{1.0};
	bool isMouseDown{ false },isMouseIn{ false };
	int deviceLostRetries{ 0 };
	ComPtr<ID2D1DeviceContext> render;
protected:
	virtual LRESULT onHitTest(WPARAM wParam, LPARAM lParam);
	virtual void onCreated() {};
	virtual void onShown() {};
	virtual void onHidden() {};
	virtual void onPaint() = 0;
	virtual void onMouseMove(const int& x, const int& y) {};
	virtual void onMouseDrag(const int& x, const int& y,const UINT_PTR& modifiers) {};
	virtual void onMouseDown(const int& x, const int& y, bool isRight) {};
	virtual void onMouseDoubleClick(const int& x, const int& y, bool isRight) { onMouseDown(x, y, isRight); };
	virtual void onMouseUp(const int& x, const int& y) {};
	virtual void onKeyDown(const UINT& key) {};
	virtual void onKeyUp() {};
	virtual void onChar(const UINT& ch) {};
	virtual void onMouseLeave() {};
	virtual void onMouseWheel(const int& x, const int& y, const short& delta) {};
	virtual void onTimer(const UINT& timerId) {};
	virtual BOOL onCursor();
	virtual LRESULT onMouseActivate() { return MA_NOACTIVATE; };
	virtual void onIME() {};
	virtual void onBlur() {};
	virtual void onDestroy() {};
	virtual void onDpiChanged() {};
private:
	std::wstring& getWinClsName();
	static LRESULT CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void mouseMove(const int& x, const int& y);
	void mouseLeave();
	void paint();
	void dpiChange(WPARAM wParam, LPARAM lParam);
private:
	ComPtr<IDXGISwapChain1>     swap;
	ComPtr<IDCompositionDevice> compDev;
	ComPtr<IDCompositionTarget> compTgt;
	ComPtr<IDCompositionVisual> compVis;
	ComPtr<ID2D1Bitmap1>        targetBmp;
};

