#pragma once
class WinBase
{
public:
	WinBase(const int& x, const int& y, const int& w, const int& h);
	~WinBase();
	void show();
	void hide();
	void refresh();
	void close();
	void move(const int& x, const int& y);
	void resize(const int& w, const int& h);
	void createWindow(const DWORD& exStyle=NULL, const DWORD& style= WS_POPUP);
	ComPtr<IDWriteTextLayout> getIconLayout(const std::wstring& icon, const float& fontSize, const float& w, const float& h);
	ComPtr<IDWriteTextLayout> getTextLayout(const std::wstring& str, const float& fontSize, const float& w, const float& h);
	void enableShadow();
	void enableAlpha();
public:
	HWND hwnd;
	int x, y, w, h;
	float dpi;
	bool isMouseDown{ false },isMouseIn{ false };
	ComPtr<ID2D1HwndRenderTarget> render;
protected:
	virtual LRESULT onHitTest(WPARAM wParam, LPARAM lParam);
	virtual void onPaint() = 0;
	virtual void onMouseMove(const int& x, const int& y) {};
	virtual void onMouseDrag(const int& x, const int& y) {};
	virtual void onMouseDown(const int& x, const int& y, bool isRight) {};
	virtual void onMouseUp(const int& x, const int& y) {};
	virtual void onKeyDown(const TCHAR& key) {};
	virtual void onMouseLeave() {};
	virtual void onMouseWheel(const int& x, const int& y, const short& delta) {};
	virtual bool onCursor();
private:
	std::wstring& getWinClsName();
	static LRESULT CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	int paint();
	void mouseMove(const int& x, const int& y);
	void mouseLeave();
private:
};

