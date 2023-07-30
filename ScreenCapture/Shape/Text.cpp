#include "Text.h"
#include "../Font.h"
#include "../Util.h"

namespace Shape {
    void Text::Draw(BLContext* paintCtx, const double& x1, const double& y1, const double& x2, const double& y2)
    {       
        static double margin = 18.0f;
        if (box.x0 == -1) {
            box.x0 = x1 - margin;
            box.y0 = y1 - fontSize / 2 - margin;
        }
        auto font = Font::Get()->fontText;
        font->setSize(fontSize);   
        auto utf8 = ConvertToUTF8(text);
        if (!isEnding) {
            BLFontMetrics fm = font->metrics();
            BLTextMetrics tm;
            BLGlyphBuffer gb;            
            gb.setUtf8Text(utf8.c_str()); //utf8.c_str()
            font->shape(gb);
            font->getTextMetrics(gb, tm);
            box.x1 = box.x0 + tm.boundingBox.x1 - tm.boundingBox.x0 + margin * 3;
            box.y1 = y1 + fontSize / 2 + margin;

            paintCtx->setStrokeStyle(color);
            paintCtx->setStrokeWidth(2.0f);
            paintCtx->strokeBox(box);

            auto count = text.size() - cursorIndex;
            auto subText = text.substr(0, cursorIndex);
            utf8 = ConvertToUTF8(subText);
            gb.setUtf8Text(utf8.c_str()); //utf8.c_str()
            font->shape(gb);
            font->getTextMetrics(gb, tm);

            if (showInputCursor) {
                auto x = box.x0 + margin + 8 + tm.boundingBox.x1 - tm.boundingBox.x0;
                paintCtx->strokeLine(x, box.y0 + margin + 8, x, box.y1 - margin);
            }
            showInputCursor = !showInputCursor;
        }        
        paintCtx->setFillStyle(color);
        paintCtx->fillUtf8Text(BLPoint(box.x0+margin, box.y0+fontSize), *font, utf8.c_str());
    }
}