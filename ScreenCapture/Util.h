#pragma once
#include "blend2d.h"
#include <string>
#include <Windows.h>
void SetBoxByPos(BLBox& box, const double& x1, const double& y1, const double& x2, const double& y2);
std::string ConvertToUTF8(const std::wstring& wstr);

