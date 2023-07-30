#include "Text.h"
#include "../Font.h"
#include "../Util.h"

namespace Shape {
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
    void Text::InsertWord(const std::wstring& word)
    {
        text = text.substr(0, cursorIndex) + word + text.substr(cursorIndex);
        cursorIndex += 1;
    }
    void Text::Draw(BLContext* paintCtx, const double& x1, const double& y1, const double& x2, const double& y2)
    {
        if (box.x0 == -1) {
            box.x0 = x1 - margin;
            box.y0 = y1 - fontSize / 2 - margin;
            box.y1 = y1 + fontSize / 2 + margin;
        }
        auto font = Font::Get()->fontText;
        font->setSize(fontSize);   
        auto utf8 = ConvertToUTF8(text);
        paintCtx->setFillStyle(color);
        paintCtx->fillUtf8Text(BLPoint(box.x0 + margin, box.y0 + fontSize), *font, utf8.c_str());

        if (!isEnding) {
            BLFontMetrics fm = font->metrics();
            BLTextMetrics tm;
            BLGlyphBuffer gb;            
            gb.setUtf8Text(utf8.c_str()); //utf8.c_str()
            font->shape(gb);
            font->getTextMetrics(gb, tm);
            box.x1 = box.x0 + tm.boundingBox.x1 - tm.boundingBox.x0 + margin * 3;
            

            paintCtx->setStrokeStyle(color);
            paintCtx->setStrokeWidth(2.0f);
            paintCtx->strokeBox(box);

            auto subText = text.substr(0, cursorIndex);
            utf8 = ConvertToUTF8(subText);
            gb.setUtf8Text(utf8.c_str()); //utf8.c_str()
            font->shape(gb);
            font->getTextMetrics(gb, tm);

            if (showInputCursor) {
                auto x = box.x0 + margin+8 + tm.boundingBox.x1 - tm.boundingBox.x0;
                paintCtx->strokeLine(x, box.y0 + margin + 8, x, box.y1 - margin);
            }
            showInputCursor = !showInputCursor;
        }         
    }
}