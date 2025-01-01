#include "d3d_device.h"

thread_local static THREAD_D3D g_d3d = { 0 };

HRESULT CreateD3D11Device(ID3D11Device** hDevice, ID3D11DeviceContext** hContext) {
    HRESULT hr = S_OK;

    // Driver types supported
    D3D_DRIVER_TYPE DriverTypes[] = { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE, };
    UINT NumDriverTypes = ARRAYSIZE(DriverTypes);

    // Feature levels supported
    D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_1 };
    UINT NumFeatureLevels = ARRAYSIZE(FeatureLevels);

    D3D_FEATURE_LEVEL FeatureLevel;


    for (UINT DriverTypeIndex = 0; DriverTypeIndex < NumDriverTypes; ++DriverTypeIndex)
    {
        hr = D3D11CreateDevice(NULL, DriverTypes[DriverTypeIndex], NULL, 0, FeatureLevels, NumFeatureLevels, D3D11_SDK_VERSION, hDevice, &FeatureLevel, hContext);
        if (SUCCEEDED(hr))
            break;
    }
    return hr;
}

HRESULT GetD3D11Device(THREAD_D3D** hDevice) {
    HRESULT hr;
    if (!g_d3d.device)
        if (FAILED(hr = CreateD3D11Device(&g_d3d.device, &g_d3d.context)))
            return hr;
    if (g_d3d.device) {
        *hDevice = &g_d3d;
        g_d3d.ref++;
        return S_OK;
    }
    return E_FAIL;
}