#include <locale>
#include <codecvt>

std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
auto str = converter.to_bytes(text.substr(0, pos));