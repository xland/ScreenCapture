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
}