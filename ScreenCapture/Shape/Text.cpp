#include "Text.h"
#include "../Font.h"
#include "../Util.h"
#include "../MainWin.h"

static double _fontSize = 68.0f;

namespace Shape {
    Text::Text():fontSize{_fontSize}
    {
        state = State::text;
    }
    Text::~Text()
    {

    }
    void Text::activeKeyboard(LONG x, LONG y)
    {
        auto win = MainWin::Get();
        if (HIMC himc = ImmGetContext(win->hwnd))
        {
            COMPOSITIONFORM comp = {};
            comp.ptCurrentPos.x = x;
            comp.ptCurrentPos.y = y;
            comp.dwStyle = CFS_FORCE_POSITION;
            ImmSetCompositionWindow(himc, &comp);
            CANDIDATEFORM cand = {};
            cand.dwStyle = CFS_CANDIDATEPOS;
            cand.ptCurrentPos.x = x;
            cand.ptCurrentPos.y = y;
            ImmSetCandidateWindow(himc, &cand);
            ImmReleaseContext(win->hwnd, himc);
        }
    }
    void Text::SetIndex(const double& x)
    {
        int tempIndex = 0;
        for (size_t i = 0; i < text.size(); i++)
        {
            auto utf8 = ConvertToUTF8(text.substr(0,text.size()-i));
            auto font = Font::Get()->fontText;
            font->setSize(fontSize);
            BLFontMetrics fm = font->metrics();
            BLTextMetrics tm;
            BLGlyphBuffer gb;
            gb.setUtf8Text(utf8.c_str()); //utf8.c_str()
            font->shape(gb);
            font->getTextMetrics(gb, tm);
            auto xPos = box.x0 + tm.boundingBox.x1 - tm.boundingBox.x0 + margin;
            if (xPos < x) {
                tempIndex = text.size() - i;
                break;
            }
        }
        cursorIndex = tempIndex;
        showInputCursor = true;
    }
    bool Text::moveCursorIndex(const bool& toLeft)
    {
        if (toLeft && cursorIndex > 0) {
            cursorIndex -= 1;
            showInputCursor = true;
            return true;
        }
        if (!toLeft && cursorIndex < text.size()) {
            cursorIndex += 1;
            showInputCursor = true;
            return true;
        }
        return false;
    }

    void Text::DeleteWord(const bool backspace)
    {
        if (backspace) {
            if (cursorIndex < 1) return;
            text = text.substr(0, cursorIndex - 1) + text.substr(cursorIndex);
            cursorIndex -= 1;
        }
        else
        {
            if (cursorIndex >= text.size()) return;
            text = text.substr(0, cursorIndex) + text.substr(cursorIndex+1);
        }
        if (text.empty()) {
            isTemp = true;
        }
        Draw(-1, -1, -1, -1);
    }
    void Text::InsertWord(const std::wstring& word)
    {
        text = text.substr(0, cursorIndex) + word + text.substr(cursorIndex);
        cursorIndex += 1;
        isTemp = false;
        needDraw = true;
        Draw(-1, -1, -1, -1);
    }
    void Text::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
    {
        if (!needDraw)
        {
            KillTimer(MainWin::Get()->hwnd, 999);
            return;
        }
        auto win = MainWin::Get();
        if (x1 == -1) {
            SetTimer(win->hwnd, 999, 660, (TIMERPROC)NULL);
        }
        auto context = Painter::Get()->paintCtx;
        context->begin(*Painter::Get()->prepareImage);
        context->clearAll();
        if (box.x0 == -1) {
            box.x0 = x1 - margin;
            box.y0 = y1 - fontSize / 2 - margin;
            box.y1 = y1 + fontSize / 2 + margin;
        }
        auto font = Font::Get()->fontText;
        font->setSize(fontSize);   
        std::string utf8;
        context->setFillStyle(color);
        if(!text.empty()){
            utf8 = ConvertToUTF8(text);
            context->fillUtf8Text(BLPoint(box.x0 + margin, box.y0 + font->metrics().ascent+margin), *font, utf8.c_str());
        }
        BLFontMetrics fm = font->metrics();
        BLTextMetrics tm;
        BLGlyphBuffer gb;
        gb.setUtf8Text(utf8.c_str()); //utf8.c_str()
        font->shape(gb);
        font->getTextMetrics(gb, tm);
        box.x1 = box.x0 + tm.boundingBox.x1 - tm.boundingBox.x0 + margin * 3;
        if (!isDraggingDragger) {
            context->setStrokeStyle(color);
            context->setStrokeWidth(2.0f);
            context->strokeBox(box);
            auto subText = text.substr(0, cursorIndex);
            utf8 = ConvertToUTF8(subText);
            gb.setUtf8Text(utf8.c_str()); //utf8.c_str()
            font->shape(gb);
            font->getTextMetrics(gb, tm);
            if (showInputCursor) {
                auto x = box.x0 + margin + 6 + tm.boundingBox.x1 - tm.boundingBox.x0;
                context->strokeLine(x, box.y0 + margin, x, box.y1 - margin);
                activeKeyboard(x, box.y1 - margin);
            }
            showInputCursor = !showInputCursor;
        }
        if (draggerIndex != -1 && !isDraggingDragger) {
            draggers[0].x0 = box.x0 - draggerSize;
            draggers[0].y0 = box.y0 - draggerSize;
            draggers[0].x1 = box.x0 + draggerSize;
            draggers[0].y1 = box.y0 + draggerSize;

            draggers[1].x0 = box.x1 - draggerSize;
            draggers[1].y0 = box.y0 - draggerSize;
            draggers[1].x1 = box.x1 + draggerSize;
            draggers[1].y1 = box.y0 + draggerSize;

            draggers[2].x0 = box.x1 - draggerSize;
            draggers[2].y0 = box.y1 - draggerSize;
            draggers[2].x1 = box.x1 + draggerSize;
            draggers[2].y1 = box.y1 + draggerSize;

            draggers[3].x0 = box.x0 - draggerSize;
            draggers[3].y0 = box.y1 - draggerSize;
            draggers[3].x1 = box.x0 + draggerSize;
            draggers[3].y1 = box.y1 + draggerSize;
            context->setStrokeStyle(BLRgba32(0, 0, 0));
            context->setStrokeWidth(2);
            context->strokeBoxArray(draggers, 4);
        }
        context->end();
        InvalidateRect(win->hwnd, nullptr, false);
    }
    bool Text::EndDraw()
    {
        auto painter = Painter::Get();
        if (!painter->isDrawing) {
            return true;
        }
        auto win = MainWin::Get();
        if (win->IsLeftBtnDown && !win->IsDoubleClick && box.contains(win->MouseDownPos.x, win->MouseDownPos.y)) {
            SetIndex(win->MouseDownPos.x);
            Draw(win->MouseDownPos.x, win->MouseDownPos.y, -1, -1);
            return false;
        }
        if (win->IsLeftBtnDown && draggerIndex != -1) {
            return false;
        }
        KillTimer(win->hwnd, 999);
        auto context = painter->paintCtx;
        context->begin(*painter->canvasImage);
        auto font = Font::Get()->fontText;
        font->setSize(fontSize);
        auto utf8 = ConvertToUTF8(text);
        context->setFillStyle(color);
        context->fillUtf8Text(BLPoint(box.x0 + margin, box.y0 + font->metrics().ascent + margin), *font, utf8.c_str());
        context->end();

        context->begin(*painter->prepareImage);
        context->clearAll();
        context->end();
        painter->isDrawing = false;
        win->state = win->preState;
        InvalidateRect(win->hwnd, nullptr, false);
        return true;
    }
    void Text::ShowDragger()
    {
        auto win = MainWin::Get();
        isDraggingDragger = false;
        InvalidateRect(win->hwnd, nullptr, false);
    }
    void Text::MouseInDragger(const double& x, const double& y)
    {
        for (int i = 0; i < 4; i++)
        {
            if (draggers[i].contains(x, y)) {
                LPCTSTR cursor = IDC_SIZENESW;
                switch (i)
                {
                    case 0: {
                        tempDraggerX = box.x1;
                        tempDraggerY = box.y1;
                        draggerIndex = i;
                        cursor = IDC_SIZENWSE;
                        break;
                    }
                    case 1:
                    {
                        tempDraggerX = box.x0;
                        tempDraggerY = box.y1;
                        draggerIndex = i;
                        cursor = IDC_SIZENESW;
                        break;
                    }
                    case 2:
                    {
                        tempDraggerX = box.x0;
                        tempDraggerY = box.y0;
                        draggerIndex = i;
                        cursor = IDC_SIZENWSE;
                        break;
                    }
                    case 3: {
                        tempDraggerX = box.x1;
                        tempDraggerY = box.y0;
                        draggerIndex = i;
                        cursor = IDC_SIZENESW;
                        break;
                    }
                }
                ChangeCursor(cursor);
                return;
            }
        }
        if (box.contains(x, y)) {
            draggerIndex = 4;
        }
        else
        {            
            draggerIndex = -1;
        }
        
    }
    void Text::DragDragger(const double& x, const double& y)
    {
        switch (draggerIndex)
        {
            case 0: {
                isDraggingDragger = true;
                box.x0 = x;
                box.y0 = y;
                auto fs = (box.y1 - box.y0) - margin * 2;
                if (fs < 16) return;
                fontSize = fs;
                _fontSize = fontSize;
                Draw(x, y, tempDraggerX, tempDraggerY);
                break;
            }
            case 1: {
                isDraggingDragger = true;
                box.x1 = x;
                box.y0 = y;
                auto fs = (box.y1 - box.y0) - margin * 2;
                if (fs < 16) return;
                fontSize = fs;
                _fontSize = fontSize;
                Draw(tempDraggerX, y, x, tempDraggerY);
                break;
            }
            case 2: {
                isDraggingDragger = true;
                box.x1 = x;
                box.y1 = y;
                auto fs = (box.y1 - box.y0) - margin * 2;
                if (fs < 16) return;
                fontSize = fs;
                _fontSize = fontSize;
                Draw(tempDraggerX, tempDraggerY, x, y);
                break;
            }
            case 3: {
                isDraggingDragger = true;
                box.x0 = x;
                box.y1 = y;
                auto fs = (box.y1 - box.y0) - margin * 2;
                if (fs < 16) return;
                fontSize = fs;
                _fontSize = fontSize;
                Draw(x, tempDraggerY, tempDraggerX, y);
                break;
            }
            case 4: {
                auto win = MainWin::Get();
                auto xSpan = x - win->MouseDownPos.x;
                auto ySpan = y - win->MouseDownPos.y;
                box.x0 += xSpan;
                box.y0 += ySpan;
                box.x1 += xSpan;
                box.y1 += ySpan;
                Draw(box.x0, box.y0, box.x1, box.y1);
                win->MouseDownPos.x = x;
                win->MouseDownPos.y = y;
                break;
            }
        }
    }
}