#pragma once
#include <windows.h>

namespace InputLangHelper {
    void SaveKeyboardLayoutForWindow(HWND hwnd);
    void RestoreSavedKeyboardLayout();
}
