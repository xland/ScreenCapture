#pragma once
class Render;
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
	void createWindow(const DWORD& exStyle=NULL, const DWORD& style=NULL);
	HRESULT createBitmap();
	void enableShadow();
	void setTimer(const UINT& elapse, const UINT& id);
	void killTimer(const UINT& id);
	void setCursor(LPCWSTR cursorName);
	ComPtr<IDWriteTextLayout> makeIconLayout(const std::wstring& code, const float& w, const float& h,const float& size,bool hAlign = true, bool VAlign = true);
	ComPtr<IDWriteTextLayout> makeTextLayout(const std::wstring& text, const float& w, const float& h, const float& size, bool hAlign = true, bool VAlign = true);
	
public:
	HWND hwnd{nullptr};
	int x, y, w, h;
	float dpi{1.0};
	bool isMouseDown{ false },isMouseIn{ false };
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
	virtual void onMouseUp(const int& x, const int& y) {};
	virtual void onKeyDown(const UINT& key) {};
	virtual void onChar(const UINT& ch) {};
	virtual void onMouseLeave() {};
	virtual void onMouseWheel(const int& x, const int& y, const short& delta) {};
	virtual void onTimer(const UINT& timerId) {};
	virtual BOOL onCursor();
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

