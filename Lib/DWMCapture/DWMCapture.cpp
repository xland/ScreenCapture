#include <Windows.h>
#include <unknwn.h>
#include "DWMCapture.h"


typedef BOOL(WINAPI* DwmGetDxSharedSurface_td) (__in HWND hwnd, __out_opt HANDLE* p1, __out_opt LUID* p2, __out_opt ULONG* p3, __out_opt ULONG* p4, __out_opt ULONGLONG* p5);

static DwmGetDxSharedSurface_td DwmGetSharedSurface = NULL;

HANDLE GetDWMSharedHandle(HWND hwnd)
{
    HANDLE surface;
    LUID adapter;
    ULONG pFmtWindow;
    ULONG pPresentFlags;
    ULONGLONG pWin32kUpdateId;

    DwmGetSharedSurface(hwnd, &surface, &adapter, &pFmtWindow, &pPresentFlags, &pWin32kUpdateId);

    return surface;
}

HRESULT DWMCapture::Init()
{
    if (!DwmGetSharedSurface) {
        auto user32 = GetModuleHandle(TEXT("USER32"));
        if (user32)
            DwmGetSharedSurface = ((DwmGetDxSharedSurface_td)GetProcAddress(user32, "DwmGetDxSharedSurface"));
        if (!DwmGetSharedSurface)
            return E_FAIL;
    }
    HRESULT hr = S_OK;

    if (FAILED(hr = CreateD3D11Device(&m_hDevice, &m_hContext)))
        return hr;

    auto& texturedesc = m_tex.desc;
    texturedesc.Usage = D3D11_USAGE_STAGING;
    texturedesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    texturedesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texturedesc.BindFlags = 0;
    texturedesc.MiscFlags = 0;
    texturedesc.MipLevels = 1;
    texturedesc.ArraySize = 1;
    texturedesc.SampleDesc.Count = 1;
    return S_OK;
}

HRESULT DWMCapture::CaptureWindow(HWND hwndTarget, BOX* box, CAPTURE_DATA* data) {
    HANDLE textureHandle = GetDWMSharedHandle(hwndTarget);
    if (!textureHandle)
        return DWM_E_COMPOSITIONDISABLED;
    ID3D11Resource* pResource = NULL;
    HRESULT hr = m_hDevice->OpenSharedResource(textureHandle, __uuidof(ID3D11Resource), (LPVOID*)&pResource);
    if (FAILED(hr))
        return hr;
    ID3D11Texture2D* tex;
    hr = pResource->QueryInterface<ID3D11Texture2D>(&tex);
    pResource->Release();
    if (FAILED(hr))
        return hr;
    D3D11_TEXTURE2D_DESC desc;
    D3D11_BOX dbox;
    RECT window_rect{}, client_rect{};
    UINT left, top;
    bool range = false;
    tex->GetDesc(&desc);
    int w = desc.Width, h = desc.Height;
    if ((UINT_PTR)box < 65536) {
        if (GetWindowRect(hwndTarget, &window_rect)) {
            if ((UINT_PTR)box == 1 && GetClientRect(hwndTarget, &client_rect)) {
                MapWindowPoints(hwndTarget, NULL, (LPPOINT)&client_rect, 1);
                range = true, w = client_rect.right, h = client_rect.bottom;
                left = client_rect.left > window_rect.left ? client_rect.left - window_rect.left : 0;
                top = client_rect.top > window_rect.top ? client_rect.top - window_rect.top : 0;
                w = client_rect.right, h = client_rect.bottom;
                dbox = D3D11_BOX{ left,top,0,left + w,top + h,1 };
            }
            else if (DwmGetWindowAttribute(hwndTarget, DWMWA_EXTENDED_FRAME_BOUNDS, &client_rect, sizeof(client_rect)) == S_OK)
                if (client_rect.left > window_rect.left) {
                    range = true, left = client_rect.left - window_rect.left, top = client_rect.top - window_rect.top;
                    w = client_rect.right - client_rect.left, h = client_rect.bottom - client_rect.top;
                    dbox = D3D11_BOX{ left,top,0,left + w,top + h,1 };
                }
        }
    }
    else if (box && box->x1 >= 0 && box->y1 >= 0 && box->x2 <= w && box->y2 <= h) {
        dbox = D3D11_BOX{ (UINT)box->x1, (UINT)box->y1, 0, (UINT)box->x2, (UINT)box->y2, 1 };
        range = true, w = dbox.right - dbox.left, h = dbox.bottom - dbox.top;
    }
    if (w != m_tex.desc.Width || h != m_tex.desc.Height || !m_tex.p) {
        if (m_tex.p) m_tex.p->Release(), m_tex.p = nullptr;
        m_tex.desc.Width = w;
        m_tex.desc.Height = h;
        if (FAILED(hr = m_hDevice->CreateTexture2D(&m_tex.desc, nullptr, &m_tex.p))) {
            tex->Release();
            return hr;
        }
    }
    if (range)
        m_hContext->CopySubresourceRegion(m_tex.p, 0, 0, 0, 0, tex, 0, &dbox);
    else
        m_hContext->CopyResource(m_tex.p, tex);
    tex->Release();

    D3D11_MAPPED_SUBRESOURCE mapped;
    if (SUCCEEDED(hr = m_hContext->Map(m_tex.p, 0, D3D11_MAP_READ_WRITE, 0, &mapped))) {
        data->pBits = (BYTE*)mapped.pData;
        data->Pitch = mapped.RowPitch;
        data->Width = w;
        data->Height = h;
        data->tick = 0;
        m_hContext->Unmap(m_tex.p, 0);
    }
    return hr;
}