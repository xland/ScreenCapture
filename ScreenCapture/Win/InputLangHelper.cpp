#include "pch.h"
#include "Win/InputLangHelper.h"

static HKL g_savedHKL = NULL;

void InputLangHelper::SaveKeyboardLayoutForWindow(HWND hwnd)
{
    g_savedHKL = NULL;
    HWND fg = GetForegroundWindow();
    if (!hwnd) hwnd = fg;
    if (hwnd) {
        DWORD threadId = GetWindowThreadProcessId(hwnd, NULL);
        if (threadId) {
            g_savedHKL = GetKeyboardLayout(threadId);
        }
    }
}

void InputLangHelper::RestoreSavedKeyboardLayout()
{
    if (g_savedHKL) {
        // 尝试把保存的 HKL 恢复到当前线程的输入上下文
        ActivateKeyboardLayout(g_savedHKL, KLF_SETFORPROCESS);
        g_savedHKL = NULL;
    }
}
