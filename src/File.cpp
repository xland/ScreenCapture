#include "File.h"
#include <format>
#include <Windows.h>

File::File()
{
}

File::~File()
{
}

std::wstring File::MakeName() {
    SYSTEMTIME localTime;
    GetLocalTime(&localTime);
    std::wstring name = std::format(L"{}{}{}{}{}{}{}.png", localTime.wYear, localTime.wMonth, localTime.wDay,
        localTime.wHour, localTime.wMinute, localTime.wSecond, localTime.wMilliseconds);
    return name;
}