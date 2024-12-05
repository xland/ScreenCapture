#pragma once

#include <string>

class Util
{
public:
	//static void setCursor(QWidget* widget, Qt::CursorShape cursor);
	static void CopyText(const std::wstring& text);
	static std::wstring CreateFileName();
	std::string ToString(std::wstring&& wstr);
};

