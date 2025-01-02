#pragma once
#define WIN32_LEAN_AND_MEAN 
#include "d3d_device.h"
#include "types.h"
#include <dwmapi.h>

class DWMCapture
{
public:
	HRESULT Init();
    HRESULT CaptureWindow(HWND hwndTarget, BOX* box, CAPTURE_DATA* data);
    void releaseTexture() {
        RESET_OBJECT(m_tex.p);
    }
    ~DWMCapture() {
        RESET_OBJECT(m_tex.p);
        RESET_OBJECT(m_hDevice);
        RESET_OBJECT(m_hContext);
    }

private:

    ID3D11Device* m_hDevice;
    ID3D11DeviceContext* m_hContext;
    HANDLE m_sharedtexture;
    struct {
        ID3D11Texture2D* p;
        D3D11_TEXTURE2D_DESC desc;
    } m_tex = { 0 };
};

