#include "pch.h"
#include <wininet.h>
#include <process.h>
#include <commctrl.h>
#include <stdio.h>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
#include <tchar.h>
#include <memory>
#include <functional>
#include <atlbase.h>
#include <codecapi.h>
#include <mfidl.h>
#include <mfapi.h>
#include <mfreadwrite.h>
#include <dshow.h>
#include <effects.h>
#include <Wmcodecdsp.h>
#include <setjmp.h>
#include <Mferror.h>
#include <optional>
#include <d2d1_2.h>
#include <d2d1_3.h>
#include <dxgi.h>
#include <wincodec.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <shlobj.h>
#include <propvarutil.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <chrono>
#include <audiosessiontypes.h>


namespace WinVideoMp4{
// {1884E6DC-2AE3-4499-A3D7-1A27E3B2AC39}
inline static const GUID MyFakeFmt = { 0x1884e6dc, 0x2ae3, 0x4499, { 0xa3, 0xd7, 0x1a, 0x27, 0xe3, 0xb2, 0xac, 0x39 } };

// {7F478D2D-9EBF-4B9A-B2F3-83800263BB86}
inline static const GUID MFVideoFormat_RGB10 = { 0x7f478d2d, 0x9ebf, 0x4b9a, { 0xb2, 0xf3, 0x83, 0x80, 0x2, 0x63, 0xbb, 0x86 } };


class AHANDLE
{
private:
    HANDLE hX = INVALID_HANDLE_VALUE;

public:

    AHANDLE()
    {
        hX = INVALID_HANDLE_VALUE;
    }
    ~AHANDLE()
    {
        Close();
    }
    AHANDLE(const AHANDLE& h)
    {
        DuplicateHandle(GetCurrentProcess(), h.hX, GetCurrentProcess(), &hX, 0, 0, DUPLICATE_SAME_ACCESS);
    }
    AHANDLE(AHANDLE&& h)
    {
        hX = h.hX;
        h.hX = INVALID_HANDLE_VALUE;
    }
    AHANDLE(HANDLE hY)
    {
        hX = hY;
    }
    AHANDLE& operator =(const AHANDLE& h)
    {
        Close();
        DuplicateHandle(GetCurrentProcess(), h.hX, GetCurrentProcess(), &hX, 0, 0, DUPLICATE_SAME_ACCESS);
        return *this;
    }
    AHANDLE& operator =(AHANDLE&& h)
    {
        Close();
        hX = h.hX;
        h.hX = INVALID_HANDLE_VALUE;
        return *this;
    }

    void Close()
    {
        if (hX != INVALID_HANDLE_VALUE)
            CloseHandle(hX);
        hX = INVALID_HANDLE_VALUE;
    }

    operator bool() const
    {
        if (hX == INVALID_HANDLE_VALUE)
            return false;
        return true;
    }
    operator HANDLE() const
    {
        return hX;
    }


};
struct REBUFFERLOCK
{
    std::recursive_mutex* mm;
    REBUFFERLOCK(std::recursive_mutex& m)
    {
        mm = &m;
        m.lock();
    }
    ~REBUFFERLOCK()
    {
        mm->unlock();
    }
};
template <typename T = float>
class MIXBUFFER
{
    T* m = 0;
    unsigned int count = 0;
public:

    friend struct REBUFFER;

    unsigned int Count() { return count; }
    MIXBUFFER(T* f = 0)
    {
        Set(f);
    }
    void Set(T* f)
    {
        m = f;
    }
    void Reset(unsigned long long samples)
    {
        if (m)
            memset(m, 0, (size_t)(samples * sizeof(T)));
        count = 0;
    }
    void Put(T* data, unsigned long long samples, bool ForceIfEmpty = false, float VOut = 1.0f)
    {
        if (!m)
            return;
        if (!samples)
            return;

        // If null, do nothing
        if (ForceIfEmpty == false && data)
        {
            bool Silence = true;
            for (unsigned long long ismp = 0; ismp < samples; ismp++)
            {
                if (data[ismp] > 0.001f || data[ismp] < -0.001f)
                {
                    Silence = false;
                    break;
                }
            }
            if (Silence)
                return;
        }
        if (ForceIfEmpty == false && !data)
            return;

        if (data)
        {
            if (count == 0 && VOut >= 0.99f && VOut <= 1.01f)
                memcpy(m, data, (size_t)(samples * sizeof(T)));
            else
            {
                for (unsigned long long s = 0; s < samples; s++)
                {
                    m[s] += data[s] * VOut;
                }
            }
        }
        else
        {
            if (count == 0)
                memset(m, 0, (size_t)(samples * sizeof(T)));
            else
            {
            }
        }
        count++;
    }
    T* Fin(unsigned long long samples, float* A = 0)
    {
        if (count <= 1 && A == 0)
        {
            count = 0;
            return m;
        }
        if (!m)
            return m;

        float V = 0;
        for (unsigned long long s = 0; s < samples; s++)
        {
            if (count > 0)
                m[s] /= (float)count;
            if (A)
            {
                auto ff = fabs(m[s]);
                if (ff <= 1.0f && ff > V)
                    V = ff;
            }
        }
        if (A && samples)
            *A = V;
        count = 0;
        return m;
    }

};
template <typename T>
inline T Peak(T* d, size_t s)
{
    T mm = 0;
    for (size_t i = 0; i < s; i++)
    {
        auto fa = fabs(d[i]);
        if (fa > mm)
            mm = fa;
    }
    return mm;
}
struct REBUFFER
{
    std::recursive_mutex m;
    std::vector<char> d;
    AHANDLE Has = CreateEvent(0, TRUE, 0, 0);
    MIXBUFFER<float> mb;

    void FinMix(size_t sz, float* A = 0)
    {
        mb.Fin(sz / sizeof(float), A);
    }

    size_t PushX(const char* dd, size_t sz, float* A = 0, float V = 1.0f)
    {
        REBUFFERLOCK l(m);
        auto s = d.size();
        d.resize(s + sz);
        if (dd)
            memcpy(d.data() + s, dd, sz);
        else
            memset(d.data() + s, 0, sz);

        char* a1 = d.data();
        a1 += s;
        mb.Set((float*)a1);
        mb.count = 1;

        SetEvent(Has);

        float* b = (float*)(d.data() + s);
        if (V > 1.01f || V < 0.99f)
        {
            auto st = sz / sizeof(float);
            for (size_t i = 0; i < st; i++)
                b[i] *= V;
        }
        if (A)
        {
            *A = Peak<float>(b, sz / sizeof(float));
        }


        return s + sz;
    }

    size_t Av()
    {
        REBUFFERLOCK l(m);
        return d.size();
    }

    size_t PopX(char* trg, size_t sz, DWORD wi = 0, bool NR = false)
    {
        if (wi)
            WaitForSingleObject(Has, wi);
        REBUFFERLOCK l(m);
        if (sz >= d.size())
            sz = d.size();
        if (sz == 0)
            return 0;
        if (trg)
            memcpy(trg, d.data(), sz);
        if (NR == false)
            d.erase(d.begin(), d.begin() + sz);
        if (d.size() == 0)
            ResetEvent(Has);
        return sz;
    }


    void Clear()
    {
        REBUFFERLOCK l(m);
        d.clear();
    }
};
template <typename T = float>
class READYBUFF
{
public:

    std::vector<T> g;

    void Add(T* d, size_t sz)
    {
        auto t = g.size();
        g.resize(t + sz);
        memcpy(g.data() + t, d, sz * sizeof(T));
    }

    bool Next(T* d, size_t sz)
    {
        if (g.empty())
            return false;
        if (!sz)
            return false;
        memcpy(d, g.data(), sz * sizeof(T));

        if (g.size() < sz)
            g.clear();
        else
            g.erase(g.begin(), g.begin() + sz);
        return true;
    }

};
inline HRESULT MFTrs(DWORD, DWORD iid, DWORD ood, CComPtr<IMFTransform> trs, CComPtr<IMFSample> s, IMFSample** sx)
{
    if (!trs)
        return E_FAIL;
    auto hr = s ? trs->ProcessInput(iid, s, 0) : S_OK;
    if (SUCCEEDED(hr) || hr == 0xC00D36B5)
    {
        bool FX = false;
        if (hr == 0xC00D36B5)
            FX = true;

        MFT_INPUT_STREAM_INFO six = {};
        trs->GetInputStreamInfo(iid, &six);

        MFT_OUTPUT_STREAM_INFO si = {};
        CComPtr<IMFMediaBuffer> bb;
        if (si.cbSize == 0)
            trs->GetOutputStreamInfo(ood, &si);
        CComPtr<IMFSample> pSample2;
        if ((si.dwFlags & MFT_OUTPUT_STREAM_PROVIDES_SAMPLES) == 0)
        {
            if (si.cbSize == 0)
            {
                return E_FAIL;
            }
            MFCreateSample(&pSample2);
            MFCreateMemoryBuffer(si.cbSize, &bb);
            pSample2->AddBuffer(bb);
        }

        MFT_OUTPUT_DATA_BUFFER db = { 0 };
        db.dwStreamID = ood;
        db.pSample = pSample2;
        if (si.dwFlags & MFT_OUTPUT_STREAM_PROVIDES_SAMPLES)
            db.pSample = 0;

        DWORD st = 0;
        hr = trs->ProcessOutput(0, 1, &db, &st);
        if (hr == 0xC00D6D72 && FX)
        {
            hr = trs->ProcessInput(iid, s, 0);
            hr = trs->ProcessOutput(0, 1, &db, &st);
        }
        if (FAILED(hr))
            return hr;
        if (db.pEvents)
            db.pEvents->Release();
        if (!db.pSample)
            return E_FAIL;
        if (sx)
        {
            *sx = db.pSample;
            if ((si.dwFlags & MFT_OUTPUT_STREAM_PROVIDES_SAMPLES) == 0)
                (*sx)->AddRef();

        }
        return S_OK;
    }
    return hr;
}
struct VISTAMIXER
{
    std::wstring id; // id
    std::wstring name; // friendly
    DWORD st = 0; // state
    bool CanCapture = 0;
    bool CanPlay = 0;
    int Mode = 0; // 0 none, 1 shared, 2 exclusive

    std::map<AUDCLNT_SHAREMODE, std::map<int, WAVEFORMATEXTENSIBLE>> maps;

    int NumChannels(AUDCLNT_SHAREMODE s)
    {
        auto mo = maps.find(s);
        if (mo == maps.end())
            return 0;
        if (mo->second.empty())
            return 0;
        return mo->second.begin()->second.Format.nChannels;
    }
    void test(CComPtr<IAudioClient> ac, bool AllShared)
    {
        if (ac == 0)
        {
            CComPtr<IMMDeviceEnumerator> deviceEnumerator = 0;
            auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)&deviceEnumerator);
            if (!deviceEnumerator)
                return;

            CComPtr<IMMDevice> d;
            deviceEnumerator->GetDevice(id.c_str(), &d);
            if (!d)
                return;
            hr = d->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (LPVOID*)&ac);
            if (!ac)
                return;
        }

        // Mix format
        WAVEFORMATEXTENSIBLE* ext = 0;
        ac->GetMixFormat((WAVEFORMATEX**)&ext);
        if (!ext)
            return;

        maps[AUDCLNT_SHAREMODE_SHARED][ext->Format.nSamplesPerSec] = *ext;
        WAVEFORMATEXTENSIBLE wfx = *ext;
        CoTaskMemFree(ext);

        if (AllShared)
        {
            for (DWORD sr : { 22050, 44100, 48000, 88200, 96000, 192000 })
            {
                if (ext->Format.nSamplesPerSec != sr)
                {
                    auto wfx2 = *ext;
                    wfx2.Format.nSamplesPerSec = sr;
                    wfx2.Format.nAvgBytesPerSec = wfx.Format.nSamplesPerSec * wfx.Format.nBlockAlign;
                    maps[AUDCLNT_SHAREMODE_SHARED][sr] = wfx2;
                }
            }
        }

        // And the exclusives
        wfx.Format.wFormatTag = WAVE_FORMAT_PCM;
        wfx.Format.cbSize = 0;
        wfx.Format.wBitsPerSample = 16;
        wfx.Format.nBlockAlign = wfx.Format.nChannels * wfx.Format.wBitsPerSample / 8;
        for (auto sr : { 22050,44100,48000,88200,96000,192000 })
        {
            wfx.Format.nSamplesPerSec = sr;
            wfx.Format.nAvgBytesPerSec = wfx.Format.nSamplesPerSec * wfx.Format.nBlockAlign;
            auto hr = ac->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE, (WAVEFORMATEX*)&wfx, 0);
            if (hr == S_OK)
                maps[AUDCLNT_SHAREMODE_EXCLUSIVE][wfx.Format.nSamplesPerSec] = wfx;
        }
    }

    bool CanSR(AUDCLNT_SHAREMODE sm, std::vector<std::tuple<int, bool, int>>& sampleRates)
    {
        auto mo = maps.find(sm);
        if (mo == maps.end())
            return false;
        if (mo->second.empty())
            return false;
        for (auto& s : sampleRates)
        {
            int sr = std::get<0>(s);
            auto m2 = mo->second.find(sr);
            if (m2 == mo->second.end())
            {
                std::get<1>(s) = 0;
                std::get<2>(s) = 0;
                if (sm == AUDCLNT_SHAREMODE_SHARED)
                {
                    std::get<1>(s) = 1;
                    std::get<2>(s) = mo->second[0].Format.nChannels;
                }
                continue;
            }
            std::get<1>(s) = 1;
            std::get<2>(s) = m2->second.Format.nChannels;
        }
        return true;
    }

};
DEFINE_PROPERTYKEY(PKEY_Device_FriendlyName, 0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 14);    // DEVPROP_TYPE_STRING
inline void EnumVistaMixers(std::vector<VISTAMIXER>& vistamixers)
{
    vistamixers.clear();

    // Windows Vista+ Mixers as well
    CComPtr<IMMDeviceEnumerator> deviceEnumerator = 0;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)&deviceEnumerator);
    if (deviceEnumerator)
    {
        CComPtr <IMMDeviceCollection> e = 0;

        // Capture
        deviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATEMASK_ALL, &e);
        if (e)
        {
            UINT c = 0;
            e->GetCount(&c);
            for (unsigned int i = 0; i < c; i++)
            {
                CComPtr<IMMDevice> d = 0;
                e->Item(i, &d);
                if (d)
                {
                    VISTAMIXER vm;
                    LPWSTR id = 0;
                    d->GetId(&id);
                    if (id)
                    {
                        vm.id = id;
                        CoTaskMemFree(id);
                    }
                    d->GetState(&vm.st);

                    CComPtr <IPropertyStore> st = 0;
                    d->OpenPropertyStore(STGM_READ, &st);
                    if (st)
                    {
                        PROPVARIANT pv;
                        PropVariantInit(&pv);
                        st->GetValue(PKEY_Device_FriendlyName, &pv);
                        if (pv.pwszVal)
                            vm.name = pv.pwszVal;
                        else
                            vm.name = L"Record Device";
                        PropVariantClear(&pv);
                    }

                    vm.CanCapture = true;
                    vm.CanPlay = false;
                    CComPtr<IAudioClient> ac = 0;
                    d->Activate(__uuidof(IAudioClient), CLSCTX_ALL, 0, (void**)&ac);
                    if (ac)
                    {
                        vm.test(ac, 0);
                        ac.Release();
                        vm.Mode = 0;
                        vistamixers.push_back(vm);
                    }
                }
            }
        }

        // Render
        e.Release();
        deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATEMASK_ALL, &e);
        if (e)
        {
            UINT c = 0;
            e->GetCount(&c);
            for (unsigned int i = 0; i < c; i++)
            {
                CComPtr<IMMDevice> d = 0;
                e->Item(i, &d);
                if (d)
                {
                    VISTAMIXER vm;
                    LPWSTR id = 0;
                    d->GetId(&id);
                    if (id)
                    {
                        vm.id = id;
                        CoTaskMemFree(id);
                    }
                    d->GetState(&vm.st);

                    CComPtr <IPropertyStore> st = 0;
                    d->OpenPropertyStore(STGM_READ, &st);
                    if (st)
                    {
                        PROPVARIANT pv;
                        PropVariantInit(&pv);
                        st->GetValue(PKEY_Device_FriendlyName, &pv);
                        if (pv.pwszVal)
                            vm.name = pv.pwszVal;
                        else
                            vm.name = L"Playback Device";
                        PropVariantClear(&pv);
                    }

                    vm.CanCapture = false;
                    vm.CanPlay = true;
                    CComPtr<IAudioClient> ac = 0;
                    d->Activate(__uuidof(IAudioClient), CLSCTX_ALL, 0, (void**)&ac);
                    if (ac)
                    {
                        vm.test(ac, 0);
                        ac.Release();
                        vm.Mode = 0;
                        vistamixers.push_back(vm);
                    }
                    d.Release();
                }
            }
        }
    }
}
class CAPTURE
{
public:

    DXGI_FORMAT InHDR = DXGI_FORMAT_UNKNOWN;
    int ConvertHDR = 2; // 0 no convert, 1 convert, 1 convert and go updown
    HRESULT CreateDirect3DDevice(IDXGIAdapter1* g)
    {
        HRESULT hr = S_OK;

        // Driver types supported
        D3D_DRIVER_TYPE DriverTypes[] =
        {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE,
        };
        UINT NumDriverTypes = ARRAYSIZE(DriverTypes);

        // Feature levels supported
        D3D_FEATURE_LEVEL FeatureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1
        };
        UINT NumFeatureLevels = ARRAYSIZE(FeatureLevels);

        D3D_FEATURE_LEVEL FeatureLevel;

        // Create device
        for (UINT DriverTypeIndex = 0; DriverTypeIndex < NumDriverTypes; ++DriverTypeIndex)
        {
            hr = D3D11CreateDevice(g, DriverTypes[DriverTypeIndex], nullptr, D3D11_CREATE_DEVICE_VIDEO_SUPPORT, FeatureLevels, NumFeatureLevels,
                D3D11_SDK_VERSION, &device, &FeatureLevel, &context);
            if (SUCCEEDED(hr))
            {
                // Device creation success, no need to loop anymore
                break;
            }
        }
        if (FAILED(hr))
            return hr;

        return S_OK;
    }



    std::vector<BYTE> buf;
    CComPtr<ID3D11Device> device;
    CComPtr<ID3D11DeviceContext> context;
    CComPtr<IDXGIOutputDuplication> lDeskDupl;
    CComPtr<ID3D11Texture2D> lGDIImage;
    CComPtr<ID3D11Texture2D> lDestImage;
    DXGI_OUTDUPL_DESC lOutputDuplDesc = {};

    static void GetAdapters(std::vector<CComPtr<IDXGIAdapter1>>& a)
    {
        CComPtr<IDXGIFactory1> df;
        CreateDXGIFactory1(__uuidof(IDXGIFactory1),(void**)&df);
        a.clear();
        if (!df)
            return;
        int L = 0;
        for (;;)
        {
            CComPtr<IDXGIAdapter1> lDxgiAdapter;
            df->EnumAdapters1(L, &lDxgiAdapter);
            if (!lDxgiAdapter)
                break;
            L++;
            a.push_back(lDxgiAdapter);
        }
        return;
    }

    bool Get(IDXGIResource* lDesktopResource,bool Curs,RECT* rcx = 0)
    {
        // QI for ID3D11Texture2D
        CComPtr<ID3D11Texture2D> lAcquiredDesktopImage;
        if (!lDesktopResource)
            return 0;
        auto hr = lDesktopResource->QueryInterface(IID_PPV_ARGS(&lAcquiredDesktopImage));
        if (!lAcquiredDesktopImage)
            return 0;
        lDesktopResource = 0;


        if (InHDR != DXGI_FORMAT_UNKNOWN || Curs == 0)
        {
            // No Cursor support
            context->CopyResource(lDestImage, lAcquiredDesktopImage);
        }
        else
        {
            // Copy image into GDI drawing texture
            context->CopyResource(lGDIImage, lAcquiredDesktopImage);

            // Draw cursor image into GDI drawing texture
            CComPtr<IDXGISurface1> lIDXGISurface1;

            lIDXGISurface1 = lGDIImage;

            if (!lIDXGISurface1)
                return 0;

            CURSORINFO lCursorInfo = { 0 };
            lCursorInfo.cbSize = sizeof(lCursorInfo);
            auto lBoolres = GetCursorInfo(&lCursorInfo);
            if (lBoolres == TRUE)
            {
                if (lCursorInfo.flags == CURSOR_SHOWING && Curs)
                {
                    auto lCursorPosition = lCursorInfo.ptScreenPos;
                    //                auto lCursorSize = lCursorInfo.cbSize;
                    HDC  lHDC;
                    lIDXGISurface1->GetDC(FALSE, &lHDC);
                    DrawIconEx(
                        lHDC,
                        lCursorPosition.x,
                        lCursorPosition.y,
                        lCursorInfo.hCursor,
                        0,
                        0,
                        0,
                        0,
                        DI_NORMAL | DI_DEFAULTSIZE);
                    lIDXGISurface1->ReleaseDC(nullptr);
                }
            }

            // Copy image into CPU access texture
            context->CopyResource(lDestImage, lGDIImage);
        }


        // Copy from CPU access texture to bitmap buffer
        D3D11_MAPPED_SUBRESOURCE resource;
        UINT subresource = D3D11CalcSubresource(0, 0, 0);
        hr = context->Map(lDestImage, subresource, D3D11_MAP_READ_WRITE, 0, &resource);
        if (FAILED(hr))
            return 0;

        int multi = 4;
        if (InHDR == DXGI_FORMAT_R16G16B16A16_FLOAT)
            multi = 8; // 64-bit

        auto sz = lOutputDuplDesc.ModeDesc.Width
            * lOutputDuplDesc.ModeDesc.Height * multi;
        auto sz2 = sz;
        buf.resize(sz);
        if (rcx)
        {
            sz2 = (rcx->right - rcx->left) * (rcx->bottom - rcx->top) * multi;
            buf.resize(sz2);
            sz = sz2;
        }

        UINT lBmpRowPitch = lOutputDuplDesc.ModeDesc.Width * multi;
        if (rcx)
            lBmpRowPitch = (rcx->right - rcx->left) * multi;
        UINT lRowPitch = std::min<UINT>(lBmpRowPitch, resource.RowPitch);

        BYTE* sptr = reinterpret_cast<BYTE*>(resource.pData);
        BYTE* dptr = buf.data() + sz - lBmpRowPitch;
        if (rcx)
            sptr += rcx->left * multi;
        for (size_t h = 0; h < lOutputDuplDesc.ModeDesc.Height; ++h)
        {
            if (rcx && h < (size_t)rcx->top)
            {
                sptr += resource.RowPitch;
                continue;
            }
            if (rcx && h >= (size_t)rcx->bottom)
                break;
            memcpy_s(dptr, lBmpRowPitch, sptr, lRowPitch);
            sptr += resource.RowPitch;
            dptr -= lBmpRowPitch;
        }
        context->Unmap(lDestImage, subresource);
        return 1;
    }

    bool Prepare(UINT Output = 0)
    {
    // Get DXGI device
        CComPtr<IDXGIDevice> lDxgiDevice;
        lDxgiDevice = device;
        if (!lDxgiDevice)
            return 0;

        // Get DXGI adapter
        HRESULT hr = 0;

        CComPtr<IDXGIAdapter> lDxgiAdapter;
        hr = lDxgiDevice->GetParent(
            __uuidof(IDXGIAdapter),
            reinterpret_cast<void**>(&lDxgiAdapter));

        if (FAILED(hr))
            return 0;

        lDxgiDevice = 0;

        // Get output
        CComPtr<IDXGIOutput> lDxgiOutput;
        hr = lDxgiAdapter->EnumOutputs(Output, &lDxgiOutput);
        if (FAILED(hr))
            return 0;

        lDxgiAdapter = 0;

        DXGI_OUTPUT_DESC lOutputDesc;
        hr = lDxgiOutput->GetDesc(&lOutputDesc);

        // QI for Output 1
        CComPtr<IDXGIOutput1> lDxgiOutput1;
        lDxgiOutput1 = lDxgiOutput;
        if (!lDxgiOutput1)
            return 0;

        lDxgiOutput = 0;

        // Create desktop duplication
        hr = lDxgiOutput1->DuplicateOutput(
            device,
            &lDeskDupl);

        if (FAILED(hr))
            return 0;

        lDxgiOutput1 = 0;

        // Create GUI drawing texture
        lDeskDupl->GetDesc(&lOutputDuplDesc);
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = lOutputDuplDesc.ModeDesc.Width;
        desc.Height = lOutputDuplDesc.ModeDesc.Height;
        desc.Format = lOutputDuplDesc.ModeDesc.Format;
        desc.ArraySize = 1;
        desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
        InHDR = DXGI_FORMAT_UNKNOWN;
        if (desc.Format == DXGI_FORMAT_R16G16B16A16_FLOAT) // HDR, no GDI compatible
        {
            InHDR = desc.Format;
        }
        else
            desc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.MipLevels = 1;
        desc.CPUAccessFlags = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        lGDIImage = 0;
        hr = device->CreateTexture2D(&desc, NULL, &lGDIImage);
        if (FAILED(hr))
        {
//            desc.MiscFlags = 0;
  //          hr = device->CreateTexture2D(&desc, NULL, &lGDIImage);
        }
        if (FAILED(hr))
            return 0;

        if (lGDIImage == nullptr)
            return 0;

        // Create CPU access texture
        desc.Width = lOutputDuplDesc.ModeDesc.Width;
        desc.Height = lOutputDuplDesc.ModeDesc.Height;
        desc.Format = lOutputDuplDesc.ModeDesc.Format;
        desc.ArraySize = 1;
        desc.BindFlags = 0;
        desc.MiscFlags = 0;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.MipLevels = 1;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
        desc.Usage = D3D11_USAGE_STAGING;
        lDestImage = 0;
        hr = device->CreateTexture2D(&desc, NULL, &lDestImage);
        if (FAILED(hr))
            return 0;

        if (lDestImage == nullptr)
            return 0;

        return 1;
    }


    CComPtr<IWICImagingFactory2> wbfact;
    HRESULT Convert(void* f, int wi, int he,std::vector<DWORD>& out,GUID from = GUID_WICPixelFormat128bppPRGBAFloat)
    {
        if (!wbfact)
            CoCreateInstance(CLSID_WICImagingFactory2, 0, CLSCTX_INPROC_SERVER,
                __uuidof(IWICImagingFactory2), (void**)&wbfact);

        int multi = 4;
        if (from == GUID_WICPixelFormat128bppPRGBAFloat)
            multi = 16;
        if (from == GUID_WICPixelFormat64bppRGBAHalf)
            multi = 8;
        CComPtr<IWICBitmap> b;
        wbfact->CreateBitmapFromMemory(wi, he, from, wi * multi, wi * he * multi, (BYTE*)f, &b);
        if (!b)
            return E_FAIL;


        CComPtr<IWICFormatConverter> wf;
        wbfact->CreateFormatConverter(&wf);

        if (!wf)
            return E_FAIL;
        auto hr = wf->Initialize(b, GUID_WICPixelFormat32bppRGBA1010102, WICBitmapDitherTypeNone, 0, 0, WICBitmapPaletteTypeCustom);
        if (FAILED(hr))
            return E_FAIL;

        CComPtr<IWICBitmap> wic;
        wbfact->CreateBitmapFromSource(wf, WICBitmapCacheOnLoad, &wic);
        if (!wic)
            return E_FAIL;

        WICRect wr = { 0,0,(INT)wi,(INT)he };
        out.resize(wi * he);
        hr = wic->CopyPixels(&wr, wi * 4, (UINT)out.size() * 4, (BYTE*)out.data());

        return hr;
    }

};
struct DESKTOPCAPTUREPARAMS
{
    bool HasVideo = 1;
    bool HasAudio = 1;
    std::vector<std::tuple<std::wstring, std::vector<int>>> AudioFrom;
    GUID VIDEO_ENCODING_FORMAT = MFVideoFormat_H264;
    GUID AUDIO_ENCODING_FORMAT = MFAudioFormat_MP3;
    std::wstring f;
    void* cb = 0;
    std::function<HRESULT(const BYTE* d, size_t sz,void* cb)> Streamer;
    std::function<HRESULT(DXGI_FORMAT dxgi,int wi,int he,const BYTE* d, size_t sz,void* cb)> Framer;
    std::function<void(IMFAttributes* a)> PrepareAttributes;
    int fps = 25;
    int NumThreads = 0;
    int Qu = -1;
    int vbrm = 0;
    int vbrq = 0;
    int BR = 4000;
    int NCH = 2;
    int SR = 44100;
    int ABR = 192;
    bool Cursor = true;
    RECT rx = { 0,0,0,0 };
    HWND hWnd = 0;
    IDXGIAdapter1* ad = 0;
    UINT nOutput = 0;

    unsigned long long StartMS = 0; // 0, none
    unsigned long long EndMS = 0; // 0, none
    bool MustEnd = false;
    bool Pause = false;
};
struct VectorStreamX2 : public IMFByteStream
{
    ULONG r = 1;
    std::vector<char> d;
    size_t p = 0;

    // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject)
    {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IMFByteStream))
        {
            *ppvObject = (IStream*)this;
            r++;
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return ++r;
    }

    virtual ULONG STDMETHODCALLTYPE Release(void)
    {
        return --r;
    }

    HRESULT __stdcall  BeginRead(
        BYTE* pb,
        ULONG            cb,
        IMFAsyncCallback* pCallback,
        IUnknown* punkState
    )
    {
        ULONG pcb = 0;
        Read(pb, cb, &pcb);
        NextCB = pcb;
        CComPtr<IMFAsyncResult> ar;
        MFCreateAsyncResult(0, pCallback, punkState, &ar);
        pCallback->Invoke(ar);
        return S_OK;
    }

    ULONG NextCB = 0;
    HRESULT __stdcall   BeginWrite(
        const BYTE* pb,
        ULONG            cb,
        IMFAsyncCallback* pCallback,
        IUnknown* punkState
    )
    {
        ULONG pcb = 0;
        Write(pb, cb, &pcb);
        NextCB = pcb;
        CComPtr<IMFAsyncResult> ar;
        MFCreateAsyncResult(0, pCallback, punkState, &ar);
        pCallback->Invoke(ar);
        return S_OK;
    }

    HRESULT __stdcall   Close(

    )
    {
        return S_OK;
    }

    HRESULT __stdcall  EndRead(
        IMFAsyncResult*,
        ULONG* pcbRead
    )
    {
        if (!pcbRead)
            return E_POINTER;
        *pcbRead = NextCB;
        return S_OK;
    }

    HRESULT __stdcall EndWrite(
        IMFAsyncResult* ,
        ULONG* pcbWritten
    )
    {
        if (!pcbWritten)
            return E_POINTER;
        *pcbWritten = NextCB;
        return S_OK;
    }

    HRESULT __stdcall  Flush()
    {
        return S_OK;
    }

    HRESULT __stdcall   GetCapabilities(
        DWORD* pdwCapabilities
    )
    {
        if (!pdwCapabilities)
            return E_POINTER;
        *pdwCapabilities = MFBYTESTREAM_IS_READABLE | MFBYTESTREAM_IS_WRITABLE | MFBYTESTREAM_IS_SEEKABLE;
        return S_OK;
    }

    HRESULT __stdcall  GetCurrentPosition(
        QWORD* pqwPosition
    )
    {
        if (!pqwPosition)
            return E_POINTER;
        *pqwPosition = p;
        return S_OK;
    }

    HRESULT __stdcall  GetLength(
        QWORD* q
    )
    {
        if (!q)
            return E_POINTER;
        *q = d.size();
        return S_OK;
    }

    HRESULT __stdcall  IsEndOfStream(
        BOOL* q
    )
    {
        if (!q)
            return E_POINTER;
        *q = FALSE;
        if (p == d.size())
            *q = TRUE;
        return S_OK;
    }

    HRESULT __stdcall  Read(
        BYTE* pv,
        ULONG cb,
        ULONG* pcbRead
    )
    {
        auto av = d.size() - p;
        if (cb < av)
            av = cb;
        memcpy(pv, d.data() + p, av);
        p += av;
        if (pcbRead)
            *pcbRead = (ULONG)av;
        if (av < cb)
            return S_FALSE;
        return S_OK;
    }

    std::function<HRESULT(const BYTE*, size_t,void* cb)> func;
    void* cbx = 0;
    HRESULT __stdcall  Write(
        const BYTE* pv,
        ULONG      cb,
        ULONG* pcbWritten
    )
    {
        if (d.size() < (p + cb))
        {
            auto exc = (p + cb) - d.size();
            d.resize(d.size() + exc);
        }
        memcpy(d.data() + p, pv, cb);

        if (func)
        {
          auto hr = func(pv, cb,cbx);
          if (FAILED(hr))
              return hr;
        }


        p += cb;
        if (pcbWritten)
            *pcbWritten = cb;
        return S_OK;
    }

    HRESULT __stdcall  SetLength(
        QWORD qwLength
    )

    {
        d.resize(qwLength);
        if (p >= qwLength)
            p = qwLength;
        return S_OK;
    }
    
    HRESULT __stdcall  SetCurrentPosition(
        QWORD q
    )
    {
        if (q > d.size())
            return E_FAIL;
        p = q;
        return S_OK;

    }


    HRESULT __stdcall Seek(
        MFBYTESTREAM_SEEK_ORIGIN dwOrigin,
        LONGLONG                 llSeekOffset,
        DWORD                    ,
        QWORD* pqwCurrentPosition
    )
    {
        LARGE_INTEGER lo = { 0 };
        if (dwOrigin == msoBegin)
        {
            p = llSeekOffset;
        }
        if (dwOrigin == msoCurrent)
        {
            p += llSeekOffset;
        }
        if (p >= d.size())
            p = d.size();
        if (pqwCurrentPosition)
            *pqwCurrentPosition = p;

        return S_OK;
    }


};

inline void setAudio(DESKTOPCAPTUREPARAMS& dp,bool useSpeaker, bool useMic) 
{
    if (!useMic && !useSpeaker) return;
    dp.AudioFrom.clear();
    ComPtr<IMMDeviceEnumerator> deviceEnumerator = nullptr;
    HRESULT hrAudio = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)&deviceEnumerator);
    if (SUCCEEDED(hrAudio) && deviceEnumerator)
    {
        if (useMic) {
            ComPtr<IMMDevice> defaultCapture = nullptr;
            if (SUCCEEDED(deviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &defaultCapture)) && defaultCapture)
            {
                LPWSTR capId = nullptr;
                if (SUCCEEDED(defaultCapture->GetId(&capId)) && capId)
                {
                    dp.AudioFrom.push_back({ capId, {0, 1} });
                    CoTaskMemFree(capId);
                }
            }
        }
        if (useSpeaker) {
            ComPtr<IMMDevice> defaultRender = nullptr;
            if (SUCCEEDED(deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultRender)) && defaultRender)
            {
                LPWSTR renId = nullptr;
                if (SUCCEEDED(defaultRender->GetId(&renId)) && renId)
                {
                    dp.AudioFrom.push_back({ renId, {0, 1} });
                    CoTaskMemFree(renId);
                }
            }
        }
    }
}

inline int DesktopCapture(DESKTOPCAPTUREPARAMS& dp)
{
    HRESULT hr = S_OK;
    struct AUDIOIN
    {
    private:
        BYTE* pData = 0;
        UINT32 framesAvailable = 0;
        DWORD  flags = 0;

    public:
        std::wstring id;
        CComPtr<IMMDevice> d;
        CComPtr<IAudioClient> ac;
        CComPtr<IAudioClient> ac2;
        CComPtr<IAudioCaptureClient> cap;
        CComPtr<IAudioRenderClient> ren;
        WAVEFORMATEXTENSIBLE wfx = {};

        std::vector<float> tempf;
        std::vector<int> chm;
        std::vector<std::shared_ptr<REBUFFER>> buffer;
        HANDLE hEv = 0;



        bool Capturing = true;
        bool CapturingFin1 = false;
        bool CapturingFin2 = false;
        std::shared_ptr<REBUFFER> AudioDataX;
        std::vector<char> PopBuffer;

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

        size_t AvFrames()
        {
            auto avs = AudioDataX->Av();
            avs /= wfx.Format.nChannels;
            avs /= (wfx.Format.wBitsPerSample / 8);
            return avs;
        }

        void PlaySilence(REFERENCE_TIME rt)
        {
            // ns
            rt /= 10000;
            // in SR , 1000 ms
            //  ?    , rt ms
            auto ns = (wfx.Format.nSamplesPerSec * rt);
            ns /= 1000;
            while (Capturing)
            {
                if (!ren)
                    break;

                Sleep((DWORD)(rt / 2));

                if (!Capturing)
                    break;

                // See how much buffer space is available.
                UINT32 numFramesPadding = 0;
                auto hr = ac2->GetCurrentPadding(&numFramesPadding);
                if (FAILED(hr))
                    break;


                auto numFramesAvailable = ns - numFramesPadding;
                if (!numFramesAvailable)
                    continue;

                BYTE* db = 0;
                hr = ren->GetBuffer((UINT32)numFramesAvailable, &db);
                if (FAILED(hr))
                    break;
                auto bs = numFramesAvailable * wfx.Format.nChannels * wfx.Format.wBitsPerSample / 8;
                memset(db, 0,(size_t) bs);
                ren->ReleaseBuffer((UINT32)numFramesAvailable, 0); //AUDCLNT_BUFFERFLAGS_SILENT
            }
            CapturingFin2 = true;
        }

        void ThreadLoopCapture()
        {
            UINT64 up, uq;
            while (Capturing)
            {
                if (hEv)
                    WaitForSingleObject(hEv, INFINITE);

                if (!Capturing)
                    break;
                auto hr = cap->GetBuffer(&pData, &framesAvailable, &flags, &up, &uq);
                if (FAILED(hr))
                    break;
                if (framesAvailable == 0)
                    continue;

                auto ThisAudioBytes = framesAvailable * wfx.Format.nChannels * wfx.Format.wBitsPerSample/8 ;

                AudioDataX->PushX((const char*)pData, ThisAudioBytes);
                cap->ReleaseBuffer(framesAvailable);
            }
            CapturingFin1 = true;
        }
    };
#ifndef TURBO_PLAY_SC
    std::vector<VISTAMIXER> vistamixers;
    EnumVistaMixers(vistamixers);
#else
    EnumVistaMixers();
#endif
/*    if (dp.HasAudio)
    {
        VM::EnumVistaMixers();
        dp.AudioFrom
            = {
        //        VM::vistamixers[0].id,VM::vistamixers[1].id,
        //        VM::vistamixers[1].id,
                {VM::vistamixers[0].id, {0,1} }
        };
    }
*/
    // ---------------------- 
    std::vector<std::shared_ptr<AUDIOIN>> ains;

    class AINRELEASE
    {
        std::vector<std::shared_ptr<AUDIOIN>>* ains = 0;
    public:
        AINRELEASE(std::vector<std::shared_ptr<AUDIOIN>>& x)
        {
            ains = &x;
        }
        ~AINRELEASE()
        {
            if (!ains)
                return;
            for (auto& a : *ains)
            {
                a->Capturing = false;
                if (a->hEv)
                    SetEvent(a->hEv);
                for (int i = 0; i < 10; i++)
                {
                    if (a->CapturingFin1 && a->CapturingFin2)
                        break;
                    Sleep(150);
                }
                if (a->hEv)
                    CloseHandle(a->hEv);
                a->hEv = 0;
                    

            }
        }

    };
    AINRELEASE arx(ains);

    REFERENCE_TIME rt;
    unsigned long long a = dp.SR/ dp.fps; // Samples per frame
    a *= 1000;
    a *= 10000;
    a /= dp.SR;
    rt = a;

    if (dp.hWnd)
    {
        GetWindowRect(dp.hWnd, &dp.rx);
        if (dp.rx.left < 0 || dp.rx.right < 0)
            dp.rx = {};
    }

    CComPtr<IMMDeviceEnumerator> deviceEnumerator = 0;
    if(dp.HasAudio)
        hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)&deviceEnumerator);

    CAPTURE cap;
    int wi = 0, he = 0;
    if (dp.HasVideo)
    {
        if (FAILED(cap.CreateDirect3DDevice(dp.ad)))
            return -1;
        if (!cap.Prepare(dp.nOutput))
            return -2;
        if (cap.InHDR)
            dp.VIDEO_ENCODING_FORMAT = MFVideoFormat_HEVC;
        wi = cap.lOutputDuplDesc.ModeDesc.Width;
        he = cap.lOutputDuplDesc.ModeDesc.Height;
        if (dp.rx.right && dp.rx.bottom)
        {
            wi = dp.rx.right - dp.rx.left;
            he = dp.rx.bottom - dp.rx.top;
        }
    }

    CComPtr<IMFAttributes> attrs;
    MFCreateAttributes(&attrs, 0);
    attrs->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, true);

    VectorStreamX2 bs;
    bs.func = dp.Streamer;
    CComPtr<IMFSinkWriter> pSinkWriter;
    if (dp.f.empty())
    {
        if (dp.Framer)
        {
            hr = S_OK;
        }
        else
        {
            if (dp.HasVideo)
                attrs->SetGUID(MF_TRANSCODE_CONTAINERTYPE, MFTranscodeContainerType_MPEG4);
            else
                attrs->SetGUID(MF_TRANSCODE_CONTAINERTYPE, MFTranscodeContainerType_MP3);
            if (dp.PrepareAttributes)
                dp.PrepareAttributes(attrs);
            hr = MFCreateSinkWriterFromURL(NULL, &bs, attrs, &pSinkWriter);
        }
    }
    else
        hr = MFCreateSinkWriterFromURL(dp.f.c_str(), NULL, attrs, &pSinkWriter);
    if (FAILED(hr)) return -3;


    CComPtr<IMFMediaType> pMediaTypeOutVideo;
    DWORD OutVideoStreamIndex = 0;
    CComPtr<IMFMediaType> pMediaTypeVideoIn;
    if (dp.HasVideo && !dp.Framer)
    {
        hr = MFCreateMediaType(&pMediaTypeOutVideo);
        if (FAILED(hr)) return -4;

        pMediaTypeOutVideo->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);

        pMediaTypeOutVideo->SetGUID(MF_MT_SUBTYPE, dp.VIDEO_ENCODING_FORMAT);

        pMediaTypeOutVideo->SetUINT32(MF_MT_AVG_BITRATE, dp.BR * 1000);
        MFSetAttributeSize(pMediaTypeOutVideo, MF_MT_FRAME_SIZE, wi, he);
        MFSetAttributeRatio(pMediaTypeOutVideo, MF_MT_FRAME_RATE, dp.fps, 1);
        MFSetAttributeRatio(pMediaTypeOutVideo, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
        pMediaTypeOutVideo->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);


        if (dp.VIDEO_ENCODING_FORMAT == MFVideoFormat_H265 || dp.VIDEO_ENCODING_FORMAT == MFVideoFormat_HEVC || dp.VIDEO_ENCODING_FORMAT == MFVideoFormat_HEVC_ES)
            pMediaTypeOutVideo->SetUINT32(MF_MT_VIDEO_NOMINAL_RANGE, MFNominalRange_Normal);

        if (dp.VIDEO_ENCODING_FORMAT == MFVideoFormat_VP80 || dp.VIDEO_ENCODING_FORMAT == MFVideoFormat_VP90)
            pMediaTypeOutVideo->SetUINT32(MF_MT_VIDEO_NOMINAL_RANGE, MFNominalRange_Normal);

/*
    MF_MT_FRAME_SIZE	1920 x 1080
    MF_MT_AVG_BITRATE	185984000
    MF_MT_MAJOR_TYPE	MFMediaType_Video
    MF_MT_VIDEO_NOMINAL_RANGE	1
    MF_MT_FRAME_RATE	30 x 1
    MF_MT_PIXEL_ASPECT_RATIO	1 x 1
    MF_MT_INTERLACE_MODE	2
    MF_MT_SUBTYPE	MFVideoFormat_HEVC


*/
        hr = pSinkWriter->AddStream(pMediaTypeOutVideo, &OutVideoStreamIndex);
        if (FAILED(hr)) return -5;
    }

    // Audio
    std::vector<HANDLE> AudioEvents;
    if (deviceEnumerator && dp.HasAudio)
    {
        for (auto& vm : vistamixers)
        {
            for (auto& xy : dp.AudioFrom)
            {
                auto& x = std::get<0>(xy);
                if (x == vm.id)
                {
                    std::shared_ptr<AUDIOIN> ain = std::make_shared<AUDIOIN>();
                    ain->AudioDataX = std::make_shared<REBUFFER>();
                    ain->id = x;

                    CComPtr<IMMDevice> d;
                    deviceEnumerator->GetDevice(x.c_str(), &d);
                    if (!d)
                        continue;

                    DWORD flg = AUDCLNT_STREAMFLAGS_NOPERSIST;
                    flg |= AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM;
                    if (vm.CanCapture)
                    {
                        flg |= AUDCLNT_STREAMFLAGS_EVENTCALLBACK;

                    }
                    if (vm.CanPlay)
                    {
                        flg |= AUDCLNT_STREAMFLAGS_LOOPBACK;
                    }

                    ain->d = d;

                    hr = d->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (LPVOID*)&ain->ac);
                    if (!ain->ac)
                        continue;


                    auto ww = vm.maps[AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED][dp.SR];
                    if (ww.Format.nSamplesPerSec == 0 && dp.SR == 44100)
                        ww = vm.maps[AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED][48000];
                    else
                        if (ww.Format.nSamplesPerSec == 0 && dp.SR == 48000)
                            ww = vm.maps[AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED][44100];

                    ww.Format.nSamplesPerSec = dp.SR;
                    ww.Format.nAvgBytesPerSec = dp.SR * ww.Format.nBlockAlign;
                    hr = ain->ac->Initialize(AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED, flg, rt, 0, (WAVEFORMATEX*)&ww, 0);
                    if (FAILED(hr))
                        continue;

                    hr = ain->ac->GetService(__uuidof(IAudioCaptureClient), (void**)&ain->cap);
                    if (!ain->cap)
                        continue;


                    AudioEvents.push_back(ain->AudioDataX->Has);
                    if (vm.CanCapture)
                    {
                        ain->hEv = CreateEvent(0, 0, 0, 0);
                        ain->ac->SetEventHandle(ain->hEv);
                    }

                    if (vm.CanPlay)
                    {
                        hr = d->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (LPVOID*)&ain->ac2);
                        if (!ain->ac2)
                            continue;


                        DWORD flg2 = AUDCLNT_STREAMFLAGS_NOPERSIST;
                        flg2 |= AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM;
//                        flg2 |= AUDCLNT_STREAMFLAGS_EVENTCALLBACK;

                        hr = ain->ac2->Initialize(AUDCLNT_SHAREMODE::AUDCLNT_SHAREMODE_SHARED, flg2, rt, 0, (WAVEFORMATEX*)&ww, 0);
                        if (FAILED(hr))
                            continue;

                        hr = ain->ac2->GetService(__uuidof(IAudioRenderClient), (void**)&ain->ren);
                        if (!ain->ren)
                            continue;
                    }

                    hr = ain->ac->Start();
                    if (FAILED(hr))
                        continue;

                    ain->wfx = ww;
                    ain->chm = std::get<1>(xy);
                    if (vm.CanPlay)
                    {
                        hr = ain->ac2->Start();
                        if (FAILED(hr))
                            continue;
                        std::thread tx2(&AUDIOIN::PlaySilence, ain.get(), rt);
                        tx2.detach();
                    }
                    else
                        ain->CapturingFin2 = true;
                    std::thread tx(&AUDIOIN::ThreadLoopCapture, ain.get());
                    tx.detach();
                    ains.push_back(ain);
                }
            }
        }

    }

    if (ains.empty())
        dp.HasAudio = 0;

    CComPtr<IMFMediaType> pMediaTypeOutAudio;
    DWORD OutAudioStreamIndex = 0;



    if (dp.HasAudio)
    {
        hr = MFCreateMediaType(&pMediaTypeOutAudio);
        if (FAILED(hr)) return -6;

        hr = pMediaTypeOutAudio->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
        if (FAILED(hr)) return -7;


        // pcm
/*        if (AUDIO_ENCODING_FORMAT == MFAudioFormat_PCM)
        {
            pMediaTypeOutAudio->SetGUID(MF_MT_SUBTYPE, vp.AUDIO_TYPE);
            pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, vp.AUDIO_CHANNELS);
            pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, vp.AUDIO_SAMPLE_RATE);
            pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, vp.AUDIO_BIT_RATE / 8);
            int BA = (int)((vp.AUDIO_INPUT_BITS / 8) * vp.AUDIO_INPUT_CHANNELS);
            pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, BA);
            pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, vp.AUDIO_INPUT_BITS);
        }
*/
        // mp3
        if (dp.AUDIO_ENCODING_FORMAT == MFAudioFormat_MP3)
        {
            pMediaTypeOutAudio->SetGUID(MF_MT_SUBTYPE, dp.AUDIO_ENCODING_FORMAT);
            pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, dp.NCH);
            pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, dp.SR);
            pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, dp.ABR*1000 / 8);
        }

/*        // wma
        if (vp.AUDIO_TYPE == MFAudioFormat_WMAudioV9)
        {
            pMediaTypeOutAudio->SetGUID(MF_MT_SUBTYPE, vp.AUDIO_TYPE);
            pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, vp.AUDIO_CHANNELS);
            pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, vp.AUDIO_SAMPLE_RATE);
        }
*/
        // aac
        if (dp.AUDIO_ENCODING_FORMAT == MFAudioFormat_AAC || dp.AUDIO_ENCODING_FORMAT == MFAudioFormat_FLAC)
        {
            pMediaTypeOutAudio->SetGUID(MF_MT_SUBTYPE, dp.AUDIO_ENCODING_FORMAT);
            pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, dp.NCH);
            pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
            pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, dp.SR);
            pMediaTypeOutAudio->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, dp.ABR*1000 / 8);
        }

        hr = pSinkWriter->AddStream(pMediaTypeOutAudio, &OutAudioStreamIndex);
        if (FAILED(hr)) return -8;
    }



    hr = MFCreateMediaType(&pMediaTypeVideoIn);
    if (FAILED(hr)) 
        return -9;
    pMediaTypeVideoIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);

    MFSetAttributeSize(pMediaTypeVideoIn, MF_MT_FRAME_SIZE, wi,he);
    MFSetAttributeRatio(pMediaTypeVideoIn, MF_MT_FRAME_RATE, dp.fps, 1);
    MFSetAttributeRatio(pMediaTypeVideoIn, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);




    // NV12 converter
    CComPtr<IMFTransform> VideoTransformMFT;
    bool NV12 = 0;
    if (false)
    {
        CComPtr<IMFTransform> trs;
        trs.CoCreateInstance(CLSID_VideoProcessorMFT);
        std::vector<DWORD> iids;
        std::vector<DWORD> oods;
        DWORD is = 0, os = 0;
        hr = trs->GetStreamCount(&is, &os);
        iids.resize(is);
        oods.resize(os);
        hr = trs->GetStreamIDs(is, iids.data(), os, oods.data());
        CComPtr<IMFMediaType> ptype;
        CComPtr<IMFMediaType> ptype2;
        MFCreateMediaType(&ptype);
        MFCreateMediaType(&ptype2);
        pMediaTypeVideoIn->CopyAllItems(ptype);
        pMediaTypeVideoIn->CopyAllItems(ptype2);

        PROPVARIANT pv;
        InitPropVariantFromCLSID(MFVideoFormat_RGB32, &pv);
        ptype->SetItem(MF_MT_SUBTYPE, pv);

        PROPVARIANT pv2;
        InitPropVariantFromCLSID(MFVideoFormat_NV12, &pv2);
        ptype2->SetItem(MF_MT_SUBTYPE, pv2);

        hr = trs->SetInputType(iids[0], ptype, 0);
        auto hr2 = trs->SetOutputType(oods[0], ptype2, 0);
        if (SUCCEEDED(hr) && SUCCEEDED(hr2))
        {
            VideoTransformMFT = trs;
            pMediaTypeVideoIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12);
            NV12 = true;
        }
    }

    if (!NV12)
        pMediaTypeVideoIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);

    if (cap.InHDR != DXGI_FORMAT_UNKNOWN)
    {
        // Force our Nvidia MFT encoder to load by calling first our fake guid
        pMediaTypeVideoIn->SetGUID(MF_MT_SUBTYPE, MyFakeFmt);
    }

    if (dp.HasVideo && !dp.Framer)
    {
        hr = pSinkWriter->SetInputMediaType(OutVideoStreamIndex, pMediaTypeVideoIn, NULL);
        if (FAILED(hr)) return -10;


        if (cap.InHDR != DXGI_FORMAT_UNKNOWN)
        {
            // Now put the real value
            pMediaTypeVideoIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB10);
            hr = pSinkWriter->SetInputMediaType(OutVideoStreamIndex, pMediaTypeVideoIn, NULL);
            if (FAILED(hr)) return -10;
        }

        CComPtr<ICodecAPI> ca;
        hr = pSinkWriter->GetServiceForStream(OutVideoStreamIndex, GUID_NULL, __uuidof(ICodecAPI), (void**)&ca);
        if (ca)
        {
            int thr2 = dp.NumThreads;
            if (thr2)
            {
                VARIANT v = {};
                v.vt = VT_UI4;
                v.ulVal = thr2;
                ca->SetValue(&CODECAPI_AVEncNumWorkerThreads, &v);
            }
            int qu = dp.Qu;
            if (qu >= 0 && qu <= 100)
            {
                VARIANT v = {};
                v.vt = VT_UI4;
                v.ulVal = qu;
                ca->SetValue(&CODECAPI_AVEncCommonQualityVsSpeed, &v);
            }

            if (dp.vbrm > 0)
            {
                VARIANT v = {};
                v.vt = VT_UI4;
                if (dp.vbrm == 1)
                    v.ulVal = eAVEncCommonRateControlMode_UnconstrainedVBR;
                if (dp.vbrm == 2)
                    v.ulVal = eAVEncCommonRateControlMode_Quality;
                if (dp.vbrm == 3)
                    v.ulVal = eAVEncCommonRateControlMode_CBR;
                ca->SetValue(&CODECAPI_AVEncCommonRateControlMode, &v);
                if (dp.vbrm == 2)
                {
                    VARIANT v7 = {};
                    v7.vt = VT_UI4;
                    v7.ulVal = dp.vbrq;
                    ca->SetValue(&CODECAPI_AVEncCommonQuality, &v7);
                }
            }
        }
    }

    CComPtr<IMFMediaType> pMediaTypeAudioIn;
    std::vector<std::wstring> TempAudioFiles;
    if (dp.HasAudio)
    {
        hr = MFCreateMediaType(&pMediaTypeAudioIn);
        if (FAILED(hr)) return -11;

        pMediaTypeAudioIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
        pMediaTypeAudioIn->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
        pMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, dp.NCH);
        pMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, dp.SR);
        pMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
        int BA = (int)((16 / 8) * dp.NCH);
        pMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, BA);
        pMediaTypeAudioIn->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, (UINT32)(dp.SR * BA));

        hr = pSinkWriter->SetInputMediaType(OutAudioStreamIndex, pMediaTypeAudioIn, NULL);
        if (FAILED(hr)) return -12;
    }


    int multi = 4;
    if (cap.InHDR != DXGI_FORMAT_UNKNOWN)
        multi = 16;

    if (!dp.Framer)
    {
        if (pSinkWriter)
            hr = pSinkWriter->BeginWriting();
        if (FAILED(hr)) return -13;
    }

    const LONG cbWidth = multi * wi;
    DWORD VideoBufferSize = cbWidth * he;
    CComPtr<IMFMediaBuffer> pVideoBuffer = NULL;
    CComPtr<IMFSample> pVideoSampleS = NULL;

    unsigned long long rtA = 0;
    unsigned long long rtV = 0;
    std::chrono::system_clock::time_point last_frame_time_point;
    bool isFirstFrame = true;

    [[maybe_unused]] unsigned long long msloop = 1000 / dp.fps;

    std::vector<char> AudioData;
    std::vector<std::vector<float>> v(dp.NCH);
    std::vector<MIXBUFFER<float>> mixv(dp.NCH);
    v.resize(dp.NCH);
    mixv.resize(dp.NCH);
   
#ifdef _DEBUG
    wchar_t ydebug[300] = { 0};
#endif
    for (;;)
    {
        if (!dp.HasAudio && !dp.Framer)
            Sleep((DWORD)msloop);
        if (dp.MustEnd)
            break;

        unsigned long long ThisDurA = 0;
        unsigned long long ThisDurV = 0;
        size_t FinalAudioBytes = 0;

        bool DirectAudioCopy = false;
/*        if (ains.size() == 1 && ains[0]->wfx.Format.nChannels == dp.NCH && ains[0]->wfx.Format.wBitsPerSample == 16)
            DirectAudioCopy = true;
*/
        if (dp.HasAudio)
        {
            // Wait for all the events
            if (!AudioEvents.empty())
                WaitForMultipleObjects((DWORD)AudioEvents.size(), AudioEvents.data(), TRUE, INFINITE);
            if (DirectAudioCopy)
            {
            }
            else
            {
                size_t AvailableSamples = 0;
                for (auto& ain : ains)
                {
                    auto frm = ain->AvFrames();
                    if (frm < AvailableSamples || AvailableSamples == 0)
                        AvailableSamples = frm;
                }

                // Pop All
                int TotalBuffers = 0;
                for (auto& ain : ains)
                {
                    if (AvailableSamples == 0)
                        break;
                    size_t ThisAudioBytes = (size_t)(AvailableSamples * ain->wfx.Format.nChannels * ain->wfx.Format.wBitsPerSample / 8);

                    ain->PopBuffer.resize(ThisAudioBytes);
                    ain->AudioDataX->PopX(ain->PopBuffer.data(), ThisAudioBytes);

                    // Convert buffer to float 
                    // shorts, interleaved
                    ain->buffer.resize(ain->chm.size());
                    for (size_t i = 0; i < ain->buffer.size(); i++)
                    {
                        auto& aa = ain->buffer[i];
                        auto& ch = ain->chm[i];
                        if (!aa)
                            aa = std::make_shared<REBUFFER>();
                        ain->tempf.resize(AvailableSamples);

                        if (ain->wfx.Format.wBitsPerSample == 16)
                        {
                            short* s = (short*)ain->PopBuffer.data();
                            for (UINT32 j = 0; j < AvailableSamples; j++)
                            {
                                short X = s[ch];
                                float V = X / 32768.0f;
                                ain->tempf[j] = V;

                                s += ain->wfx.Format.nChannels;
                            }
                        }
                        if (ain->wfx.Format.wBitsPerSample == 32)
                        {
                            float* s = (float*)ain->PopBuffer.data();
                            for (UINT32 j = 0; j < AvailableSamples; j++)
                            {
                                float X = s[ch];
                                float V = X;
                                ain->tempf[j] = V;
                                s += ain->wfx.Format.nChannels;
                            }
                        }

                        aa->PushX((const char*)ain->tempf.data(), AvailableSamples * sizeof(float));
                        TotalBuffers++;
                    }
                }

                for (int i = 0; i < dp.NCH; i++)
                    v[i].resize(AvailableSamples);

                if (TotalBuffers == dp.NCH)
                {
                    // Direct Copy to v
                    int ib = 0;
                    for (auto& ain : ains)
                    {
                        for (auto& b : ain->buffer)
                        {
                            if (!AvailableSamples)
                                continue;
                            v[ib].resize(AvailableSamples);
                            b->PopX((char*)v[ib].data(), AvailableSamples * sizeof(float));
                            ib++;
                        }
                    }
                }
                else
                {
                    // Mix to all
                    for (int ich = 0; ich < dp.NCH; ich++)
                    {
                        mixv[ich].Set(v[ich].data());
                        mixv[ich].Reset(AvailableSamples);
                    }

                    for (auto& ain : ains)
                    {
                        if (AvailableSamples == 0)
                            continue;
                        for (auto& b : ain->buffer)
                        {
                            if (!AvailableSamples)
                                continue;
                            ain->tempf.resize(AvailableSamples);
                            b->PopX((char*)ain->tempf.data(), AvailableSamples * sizeof(float));

                            for (int ich = 0; ich < dp.NCH; ich++)
                                mixv[ich].Put(ain->tempf.data(), AvailableSamples);
                        }
                    }
                    for (int ich = 0; ich < dp.NCH; ich++)
                        mixv[ich].Fin(AvailableSamples);
                }


                // v to shorts
                FinalAudioBytes = AvailableSamples * dp.NCH * 2;
                AudioData.resize(FinalAudioBytes);
                short* ad = (short*)AudioData.data();
                for (size_t i = 0; i < AvailableSamples; i++)
                {
                    for (int nch = 0; nch < dp.NCH; nch++)
                    {
                        float V = v[nch][i];
                        short S = (short)(V * 32768.0f);
                        ad[i * dp.NCH + nch] = S;
                    }
                }

                // In SR , 1000 ms
                // In ThisAudioSamples, ? 
                unsigned long long  ms = 1000 * AvailableSamples;
                ms *= 10000;
                ms /= dp.SR;
                ThisDurA = ms;
            }



/*
            if (DirectAudioCopy)
            {
                hr = ains[0].cap->GetBuffer(&ains[0].pData, &ains[0].framesAvailable, &ains[0].flags, NULL, NULL);

                ThisAudioSamples = ains[0].framesAvailable;
                ThisAudioBytes = ains[0].framesAvailable * dp.NCH * 2;
                AudioData.resize(ThisAudioBytes);
                memcpy(AudioData.data(), ains[0].pData, ThisAudioBytes);

                ains[0].cap->ReleaseBuffer(ains[0].framesAvailable);

                // In SR , 1000 ms
                // In ThisAudioSamples, ? 
                unsigned long long  ms = 1000 * ThisAudioSamples;
                ms *= 10000;
                ms /= dp.SR;
                ThisDurA = ms;
                ThisDurV = ms;
            }
           
*/
        }

        ThisDurV = 10 * 1000 * 1000 / dp.fps;
        if (ThisDurA)
            ThisDurV = ThisDurA;

        if (dp.HasAudio && ThisDurA == 0)
            continue;

        if (dp.Pause)
            continue;

        CComPtr<IDXGIResource> lDesktopResource;
        DXGI_OUTDUPL_FRAME_INFO lFrameInfo;

        // Get new frame
        BYTE* pData = NULL;
        HRESULT hrf = S_FALSE;
        if (dp.HasVideo)
        {
            hr = cap.lDeskDupl->AcquireNextFrame(
                0,
                &lFrameInfo,
                &lDesktopResource);
            if (hr == DXGI_ERROR_WAIT_TIMEOUT)
                hr = S_OK;
            if (hr == DXGI_ERROR_ACCESS_LOST)
            {
                cap.lDeskDupl = 0;
                bool C = false;
                for (int i = 0; i < 10; i++)
                {
                    if (cap.Prepare(dp.nOutput))
                    {
                        C = true;
                        break;
                    }
                    Sleep(250);
                }
                if (!C)
                    break;
                hr = S_OK;
            }
            if (FAILED(hr))
                break;

                

            // take a time stamp here
            // Get the current time point
            if (lDesktopResource && !cap.Get(lDesktopResource, dp.Cursor, dp.rx.right && dp.rx.bottom ? &dp.rx : 0))
                break;

            if (isFirstFrame)
            {
                isFirstFrame = false;
                rtV = 0;
                last_frame_time_point = std::chrono::system_clock::now();
            }
            else
            {
                // Time stamp of current frame.
                // Calculate the dt with last frame. and set to the frame property.
                auto current_frameTimePoint = std::chrono::system_clock::now();
                auto frameDuration = current_frameTimePoint - last_frame_time_point;
                last_frame_time_point = current_frameTimePoint;
                ThisDurV = frameDuration.count();
                rtV += ThisDurV;
            }


            std::vector<DWORD> hdrout;


            if (cap.InHDR == DXGI_FORMAT_R32G32B32A32_FLOAT)
            {
                // This is a 16-bit 4xfloat sample
                void* fld = cap.buf.data();
                auto hr2 = cap.Convert(fld, wi, he, hdrout, GUID_WICPixelFormat128bppPRGBAFloat);
                if (FAILED(hr2))
                    break;
                VideoBufferSize = (DWORD)(hdrout.size() * 4);
            }
            if (cap.InHDR == DXGI_FORMAT_R16G16B16A16_FLOAT && cap.ConvertHDR)
            {
                // 64-bit float 16 bit each float, convert 
                void* fld = cap.buf.data();
                auto hr2 = cap.Convert(fld, wi, he, hdrout, GUID_WICPixelFormat64bppRGBAHalf);
                if (FAILED(hr2))
                    break;
                VideoBufferSize = (DWORD)(hdrout.size() * 4);
                // Up down

                if (cap.ConvertHDR == 2)
                {
                    std::vector<DWORD> raw3;
                    raw3.resize(wi);
                    for (int y = 0; y < he / 2; y++)
                    {
                        DWORD* d = hdrout.data() + y * wi;
                        DWORD* d2 = hdrout.data() + (he - y - 1) * wi;
                        memcpy(raw3.data(), d, wi * sizeof(DWORD));
                        memcpy(d, d2, wi * sizeof(DWORD));
                        memcpy(d2, raw3.data(), wi * sizeof(DWORD));
                    }
                }
            }



            if (!pVideoBuffer)
            {
                hr = MFCreateMemoryBuffer(VideoBufferSize, &pVideoBuffer);
                if (FAILED(hr)) break;
            }

            hr = pVideoBuffer->Lock(&pData, NULL, NULL);
            if (FAILED(hr))
                break;

            if (cap.InHDR != DXGI_FORMAT_UNKNOWN)
            {
                memcpy(pData, hdrout.data(), VideoBufferSize);
            }
            else
            {
                memcpy(pData, cap.buf.data(), std::min(cap.buf.size(), (size_t)VideoBufferSize));
            }
            if (dp.Framer)
                hrf = dp.Framer(cap.InHDR,wi,he,pData, std::min(cap.buf.size(), (size_t)VideoBufferSize), dp.cb);

            hr = pVideoBuffer->Unlock();
            if (FAILED(hr)) break;

            hr = pVideoBuffer->SetCurrentLength(VideoBufferSize);
            if (FAILED(hr)) break;

            CComPtr<IMFSample> pVideoSample = NULL;

            if (!pVideoSampleS)
            {
                hr = MFCreateSample(&pVideoSampleS);
                if (FAILED(hr)) break;
            }
            pVideoSample = pVideoSampleS;


            pVideoSample->RemoveAllBuffers();
            hr = pVideoSample->AddBuffer(pVideoBuffer);
            if (FAILED(hr)) break;

            cap.lDeskDupl->ReleaseFrame();




            hr = pVideoSample->SetSampleTime(rtV);
            if (FAILED(hr)) break;

            hr = pVideoSample->SetSampleDuration(ThisDurV);
            if (FAILED(hr)) break;


            // Video Transform MFT
            if (VideoTransformMFT)
            {
                CComPtr<IMFSample> s2;
                MFTrs(0, 0, 0, VideoTransformMFT, pVideoSample, &s2);
                if (s2)
                    pVideoSample = s2;
            }

            if (pVideoSample && dp.HasVideo && !dp.Framer)
            {
                hr = pVideoSample->SetSampleTime(rtV);
                if (FAILED(hr)) break;

                hr = pVideoSample->SetSampleDuration(ThisDurV);
                if (FAILED(hr)) break;
                hr = pSinkWriter->WriteSample(OutVideoStreamIndex, pVideoSample);
                if (FAILED(hr)) break;
            }
        }

        // Audio 
        CComPtr<IMFMediaBuffer> pAudioBuffer = NULL;
        if (dp.HasAudio && FinalAudioBytes)
        {
            hr = MFCreateMemoryBuffer((DWORD)FinalAudioBytes, &pAudioBuffer);
            if (FAILED(hr))
                break;

            hr = pAudioBuffer->Lock(&pData, NULL, NULL);
            if (FAILED(hr))
                break;

            memset(pData, 0, FinalAudioBytes);
            memcpy(pData, AudioData.data(), FinalAudioBytes);

            hr = pAudioBuffer->Unlock();
            if (FAILED(hr))
                break;

            hr = pAudioBuffer->SetCurrentLength((DWORD)FinalAudioBytes);
            if (FAILED(hr))
                break;

            CComPtr<IMFSample> pAudioSample = NULL;
            hr = MFCreateSample(&pAudioSample);
            if (FAILED(hr))
                break;

            hr = pAudioSample->AddBuffer(pAudioBuffer);
            if (FAILED(hr))
                break;

            hr = pAudioSample->SetSampleTime(rtA);
            if (FAILED(hr))
                break;

            hr = pAudioSample->SetSampleDuration(ThisDurA);
            if (FAILED(hr))
                break;

#ifdef _DEBUG
            swprintf_s(ydebug, 300, L"Writing audio at %llu - %llu...", rtA, ThisDurA);
            OutputDebugString(ydebug);
#endif
            hr = pSinkWriter->WriteSample(OutAudioStreamIndex, pAudioSample);
#ifdef _DEBUG
            OutputDebugString(L"OK\r\n");
#endif
            if (FAILED(hr))
                break;

            rtA += ThisDurA;
        }

        // rtv to ms
        auto rvx = rtV / 10000;
        if (dp.EndMS > 0 && rvx >= dp.EndMS)
            break;

        if (dp.Framer && hrf == S_OK)
            break;
    }

    // Audio off
    for (auto& a6 : ains)
    {
        if (a6->ac)
            a6->ac->Stop();
    }

    if (pSinkWriter)
        hr = pSinkWriter->Finalize();
    if (FAILED(hr))
        return -14;
	return 0;
}

}