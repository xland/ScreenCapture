

#include "NativeEventFilter.h"
#include "App.h"
#include "WinBoard.h"

NativeEventFilter::NativeEventFilter() : QAbstractNativeEventFilter()
{}

NativeEventFilter::~NativeEventFilter()
{}

bool NativeEventFilter::nativeEventFilter(const QByteArray & eventType, void* message, qintptr* result)
{
    auto board = App::getFullBoard();
    if (board && board->hwnd && eventType == "windows_generic_MSG") {
        MSG* msg = static_cast<MSG*>(message);
        if (board->hwnd == msg->hwnd && msg->message == WM_WINDOWPOSCHANGING || msg->message == WM_WINDOWPOSCHANGED) {
            WINDOWPOS* pos = reinterpret_cast<WINDOWPOS*>(msg->lParam);            
            if (!(pos->flags & SWP_NOZORDER) && (pos->hwndInsertAfter != HWND_BOTTOM))
            {
                pos->hwndInsertAfter = HWND_BOTTOM;
                return 0;
            }
        }
    }
    return false;
}
