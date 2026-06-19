#pragma once

#include <windows.h>
#include <windowsx.h>
#include <winuser.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <wrl.h>
#include <wincodec.h>

#include <d2d1.h>
#include <d2d1_1.h>
#include <dxgi1_3.h>
#include <d3d11_4.h>
#include <d2d1_3.h>
#include <d2d1_3helper.h>
#include <dcomp.h>
#include <dwrite_3.h>
#include <dwmapi.h>

#include <string>
#include <string_view>
#include <ranges>
#include <algorithm>
#include <cwctype>
#include <format>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <functional>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <math.h>
#include <winrt/Windows.Data.Json.h>

using namespace Microsoft::WRL;
using namespace winrt::Windows::Data::Json;

template<typename... Args>
void log(std::wstring_view fmt, Args&&... args)
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_s(&tm, &time);
    std::wstringstream ss;
    ss << std::put_time(&tm, L"%Y-%m-%d %H:%M:%S");
    auto timeStr = ss.str();
    auto msg = std::vformat(fmt, std::make_wformat_args(args...));
    std::wstring final = L"[" + timeStr + L"] " + msg + L"\n";
    OutputDebugString(final.c_str());
}

