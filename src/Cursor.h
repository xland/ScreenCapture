#pragma once

#include "Windows.h"

namespace Cursor
{
    static inline void All() {
        SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
    }
    static inline void LeftTopRightBottom() {
        SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
    }
    static inline void TopBottom() {
        SetCursor(LoadCursor(nullptr, IDC_SIZENS));
    }
    static inline void LeftBottomRightTop() {
        SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
    }
    static inline void LeftRight() {
        SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
    }
    static inline void Text() {
        SetCursor(LoadCursor(nullptr, IDC_IBEAM));
    }
    static inline void Hand() {
        SetCursor(LoadCursor(nullptr, IDC_HAND));
    }
    static inline void Arrow() {
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
    }
    static inline void Cross() {
        SetCursor(LoadCursor(nullptr, IDC_CROSS));
    }
    enum class cursor
    {
        all,
        wnse,
        ns,
        nesw,
        we,
        arrow,
        input,
        hand,
    };

    static void SetCursor(cursor cursorType = cursor::arrow) {
        switch (cursorType)
        {
        case cursor::all:
            SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
            break;
        case cursor::wnse:
            SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
            break;
        case cursor::ns:
            SetCursor(LoadCursor(nullptr, IDC_SIZENS));
            break;
        case cursor::nesw:
            SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
            break;
        case cursor::we:
            SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
            break;
        case cursor::input:
            SetCursor(LoadCursor(nullptr, IDC_IBEAM));
            break;
        case cursor::hand:
            SetCursor(LoadCursor(nullptr, IDC_HAND));
            break;
        default:
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
            break;
        }
    }
}