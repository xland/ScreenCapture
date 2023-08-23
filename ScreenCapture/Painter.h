#pragma once
#include <Windows.h>
#include <d2d1_3.h>
#include <dcomp.h>
#include <d3d11.h>

class Painter
{
public:
	Painter(const HWND& hwnd);
	~Painter();
	void OnResize(HWND hWnd, UINT nWidth, UINT nHeight);
	void Paint(unsigned int& w, unsigned int& h, unsigned char* data,unsigned int& stride);
private:
	HRESULT CreateD2D1Factory();
	HRESULT CreateSwapChain(HWND hWnd);
	HRESULT ConfigureSwapChain(HWND hWnd);
	HRESULT CreateD3D11Device();
	HRESULT CreateDirectComposition(HWND hWnd);

	ID2D1Factory1* m_pD2DFactory1 = NULL;
	ID3D11Device* m_pD3D11Device = NULL;
	ID3D11DeviceContext* m_pD3D11DeviceContext = NULL;
	IDXGIDevice1* m_pDXGIDevice = NULL;
	ID2D1Device* m_pD2DDevice = NULL;
	ID2D1DeviceContext3* m_pD2DDeviceContext3 = NULL;
	IDXGISwapChain1* m_pDXGISwapChain1 = NULL;
	ID2D1Bitmap1* m_pD2DTargetBitmap = NULL;

	IDCompositionDevice* m_pDCompositionDevice = NULL;
	IDCompositionTarget* m_pDCompositionTarget = NULL;
};

