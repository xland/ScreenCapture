#include "Util.h"

void SetBoxByPos(BLBox& box, const double& x1, const double& y1, const double& x2, const double& y2)
{
    if (x1 < x2) {
        box.x0 = x1;
        box.x1 = x2;
    }
    else
    {
        box.x0 = x2;
        box.x1 = x1;
    }
    if (y1 < y2) {
        box.y0 = y1;
        box.y1 = y2;
    }
    else
    {
        box.y0 = y2;
        box.y1 = y1;
    }
}

void SetBoxByPosSquare(BLBox& box, const double& x1, const double& y1, const double& x2, const double& y2)
{
    if (x1 < x2) {
        box.x0 = x1;
        box.x1 = x2;
    }
    else
    {
        box.x0 = x2;
        box.x1 = x1;
    }
    if (y1 < y2) {
        box.y0 = y1;
        box.y1 = y2;
    }
    else
    {
        box.y0 = y2;
        box.y1 = y1;
    }
    auto h = box.y1 - box.y0;
    auto w = box.x1 - box.x0;
    if (w == h) {
        return;
    }
    else
    {
        if (x1 < x2) {
            box.x0 = box.x1 - h;
        }
        else
        {
            box.x1 = box.x0 + h;
        }
    }

}

std::string ConvertToUTF8(const std::wstring& wstr)
{
    const int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
    return str;
}

void ChangeCursor(LPCTSTR cursor)
{
    auto hCursor = LoadCursor(NULL, cursor);
    SetCursor(hCursor);
}
