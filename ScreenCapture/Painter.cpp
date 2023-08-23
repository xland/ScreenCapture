#include "Painter.h"

template <class T> void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

Painter::Painter(const HWND& hwnd) {
	HRESULT hr = CreateD2D1Factory();
	if (SUCCEEDED(hr))
	{
		hr = CreateD3D11Device();
		hr = CreateSwapChain(NULL);
		if (SUCCEEDED(hr))
		{
			hr = ConfigureSwapChain(hwnd);
			hr = CreateDirectComposition(hwnd);
		}		
	}
}
Painter::~Painter() {
	SafeRelease(&m_pD2DDevice);
	SafeRelease(&m_pD2DDeviceContext3);
	SafeRelease(&m_pD2DTargetBitmap);
	SafeRelease(&m_pDXGISwapChain1);
	SafeRelease(&m_pDXGIDevice);
	SafeRelease(&m_pD3D11Device);
	SafeRelease(&m_pD3D11DeviceContext);
	SafeRelease(&m_pD2DFactory1);
	SafeRelease(&m_pDCompositionDevice);
	SafeRelease(&m_pDCompositionTarget);
}

void Painter::Paint(unsigned int& w, unsigned int& h, unsigned char* data, unsigned int& stride) {
	auto rect = D2D1::RectU(0, 0, w, h);
	m_pD2DTargetBitmap->CopyFromMemory(&rect, data, stride);
	HRESULT hr = m_pDXGISwapChain1->Present(1, 0);
}
void Painter::OnResize(HWND hWnd, UINT nWidth, UINT nHeight)
{
	if (m_pDXGISwapChain1)
	{
		HRESULT hr = S_OK;
		if (nWidth != 0 && nHeight != 0)
		{
			m_pD2DDeviceContext3->SetTarget(nullptr);
			SafeRelease(&m_pD2DTargetBitmap);
			hr = m_pDXGISwapChain1->ResizeBuffers(
				2, // Double-buffered swap chain.
				nWidth, nHeight,
				DXGI_FORMAT_B8G8R8A8_UNORM,
				0
			);
			if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
			{
				CreateD3D11Device();
				CreateSwapChain(NULL);
				return;
			}
			else
			{
				//DX::ThrowIfFailed(hr);
			}
			ConfigureSwapChain(hWnd);
		}
	}
}

HRESULT Painter::CreateD2D1Factory()
{
	HRESULT hr = S_OK;
	D2D1_FACTORY_OPTIONS options;
	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));
	options.debugLevel = D2D1_DEBUG_LEVEL::D2D1_DEBUG_LEVEL_INFORMATION;
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, (void**)&m_pD2DFactory1);
	return hr;
}

HRESULT Painter::CreateSwapChain(HWND hWnd)
{
	HRESULT hr = S_OK;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
	swapChainDesc.Width = 1;
	swapChainDesc.Height = 1;
	swapChainDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1; // don't use multi-sampling
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2; // use double buffering to enable flip
	swapChainDesc.Scaling = (hWnd != NULL) ? DXGI_SCALING::DXGI_SCALING_NONE : DXGI_SCALING::DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.Flags = 0;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
	IDXGIAdapter* pDXGIAdapter = nullptr;
	hr = m_pDXGIDevice->GetAdapter(&pDXGIAdapter);
	if (SUCCEEDED(hr))
	{
		IDXGIFactory2* pDXGIFactory2 = nullptr;
		hr = pDXGIAdapter->GetParent(IID_PPV_ARGS(&pDXGIFactory2));
		if (SUCCEEDED(hr))
		{
			if (hWnd != NULL)
			{
				hr = pDXGIFactory2->CreateSwapChainForHwnd(m_pD3D11Device, hWnd, &swapChainDesc, nullptr, nullptr, &m_pDXGISwapChain1);
			}
			else
			{
				hr = pDXGIFactory2->CreateSwapChainForComposition(m_pD3D11Device, &swapChainDesc, nullptr, &m_pDXGISwapChain1);
			}
			if (SUCCEEDED(hr))
				hr = m_pDXGIDevice->SetMaximumFrameLatency(1);
			SafeRelease(&pDXGIFactory2);
		}
		SafeRelease(&pDXGIAdapter);
	}
	return hr;
}

HRESULT Painter::ConfigureSwapChain(HWND hWnd)
{
	HRESULT hr = S_OK;
	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE::D2D1_ALPHA_MODE_IGNORE), //D2D1_ALPHA_MODE::D2D1_ALPHA_MODE_PREMULTIPLIED
		0,
		0,
		NULL
	);
	unsigned int nDPI = GetDpiForWindow(hWnd);
	bitmapProperties.dpiX = nDPI;
	bitmapProperties.dpiY = nDPI;
	IDXGISurface* pDXGISurface;
	if (m_pDXGISwapChain1)
	{
		hr = m_pDXGISwapChain1->GetBuffer(0, IID_PPV_ARGS(&pDXGISurface));
		if (SUCCEEDED(hr))
		{
			hr = m_pD2DDeviceContext3->CreateBitmapFromDxgiSurface(pDXGISurface, bitmapProperties, &m_pD2DTargetBitmap);

			if (SUCCEEDED(hr))
			{
				m_pD2DDeviceContext3->SetTarget(m_pD2DTargetBitmap);
			}
			SafeRelease(&pDXGISurface);
		}
	}
	return hr;
}

// https://docs.microsoft.com/en-us/windows/win32/direct2d/devices-and-device-contexts
HRESULT Painter::CreateD3D11Device()
{
	HRESULT hr = S_OK;
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;

	// This array defines the set of DirectX hardware feature levels this app  supports.
	// The ordering is important and you should  preserve it.
	// Don't forget to declare your app's minimum required feature level in its
	// description.  All apps are assumed to support 9.1 unless otherwise stated.
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	D3D_FEATURE_LEVEL featureLevel;
	hr = D3D11CreateDevice(
		nullptr,                    // specify null to use the default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		creationFlags,              // optionally set debug and Direct2D compatibility flags
		featureLevels,              // list of feature levels this app can support
		ARRAYSIZE(featureLevels),   // number of possible feature levels
		D3D11_SDK_VERSION,
		&m_pD3D11Device,                    // returns the Direct3D device created
		&featureLevel,            // returns feature level of device created
		&m_pD3D11DeviceContext                    // returns the device immediate context
	);
	if (SUCCEEDED(hr))
	{
		// Obtain the underlying DXGI device of the Direct3D11 device.
		hr = m_pD3D11Device->QueryInterface((IDXGIDevice1**)&m_pDXGIDevice);
		if (SUCCEEDED(hr))
		{
			// Obtain the Direct2D device for 2-D rendering.
			hr = m_pD2DFactory1->CreateDevice(m_pDXGIDevice, &m_pD2DDevice);
			if (SUCCEEDED(hr))
			{
				// Get Direct2D device's corresponding device context object.
				ID2D1DeviceContext* pD2DDeviceContext = NULL;
				hr = m_pD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &pD2DDeviceContext);
				if (SUCCEEDED(hr))
					hr = pD2DDeviceContext->QueryInterface((ID2D1DeviceContext3**)&m_pD2DDeviceContext3);
				SafeRelease(&pD2DDeviceContext);
			}
		}
	}
	return hr;
}

HRESULT Painter::CreateDirectComposition(HWND hWnd)
{
	HRESULT hr = S_OK;
	hr = DCompositionCreateDevice(m_pDXGIDevice, __uuidof(m_pDCompositionDevice), (void**)(&m_pDCompositionDevice));
	if (SUCCEEDED(hr))
	{
		hr = m_pDCompositionDevice->CreateTargetForHwnd(hWnd, true, &m_pDCompositionTarget);
		if (SUCCEEDED(hr))
		{
			IDCompositionVisual* pDCompositionVisual = NULL;
			hr = m_pDCompositionDevice->CreateVisual(&pDCompositionVisual);
			if (SUCCEEDED(hr))
			{
				hr = pDCompositionVisual->SetContent(m_pDXGISwapChain1);
				hr = m_pDCompositionTarget->SetRoot(pDCompositionVisual);
				hr = m_pDCompositionDevice->Commit();
				SafeRelease(&pDCompositionVisual);
			}
		}
	}
	return hr;
}
