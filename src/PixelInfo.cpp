#include "PixelInfo.h"
#include "App.h"
#include "WindowBase.h"
#include "include/core/SkRegion.h"
#include "include/core/SkColor.h"
#include "string"
#include "format"
#include <sstream>
#include <algorithm>

PixelInfo* pixelInfo;

PixelInfo::~PixelInfo()
{
}

void PixelInfo::init()
{
    if (!pixelInfo)
    {
        pixelInfo = new PixelInfo();
    }
}

PixelInfo* PixelInfo::get()
{
    return pixelInfo;
}

bool PixelInfo::OnMouseMove(const int& x, const int& y)
{
    auto win = App::GetWin();
    if (win->state >= State::mask)
    {
        return false;
    }
    float width = 200.0f;
    float height = 200.0f;
    rect.setXYWH(x + 10, y + 10, width, height);    
    if (rect.fLeft + width > win->w) {
        rect.offset(0 - width - 20, 0);
    }
    if (rect.fTop + height > win->h) {
        rect.offset(0, 0 - height - 20);
    }
    auto canvas = win->surfaceFront->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    SkPaint paint;
    paint.setColor(SkColorSetARGB(168, 0, 0, 0));
    canvas->drawRect(rect, paint);
    float x2, y2{rect.fTop};
    for (int y1 = -5; y1 < 5; y1++)
    {
        for (int x1 = -10; x1 < 10; x1++) 
        {
            if (x1 == -10) x2 = rect.fLeft;
            auto tempX{ x + x1 }, tempY{ y + y1 };
            SkColor color{SK_ColorBLACK};
            if (tempX >= 0 && tempX < win->w && tempY >= 0 && tempY < win->h) {
                color = win->pixSrc->getColor(x + x1, y + y1);
            }            
            paint.setColor(color);
            canvas->drawRect(SkRect::MakeXYWH(x2,y2,10,10), paint);
            x2 += 10;
        }
        y2 += 10;
    }
    {
        paint.setColor(SkColorSetARGB(110, 22, 119, 255));
        SkRegion region1;
        region1.setRect(SkIRect::MakeXYWH(rect.fLeft + 95, rect.fTop, 10, 100));
        SkRegion region2;
        region2.setRect(SkIRect::MakeXYWH(rect.fLeft, rect.fTop + 45, 200, 10));
        SkRegion region3;
        region3.op(region1, region2, SkRegion::Op::kXOR_Op);
        canvas->drawRegion(region3, paint);
    }
    paint.setStyle(SkPaint::Style::kStroke_Style);
    paint.setColor(SK_ColorBLACK);
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    canvas->drawRect(rect, paint);
    paint.setColor(SK_ColorWHITE);
    paint.setStroke(false);
    auto font = App::GetFontText();
    font->setSize(14);

    std::string str = std::format("Position: X:{}  Y:{}", x, y);
    auto data = str.data();
    canvas->drawSimpleText(data, str.size(), SkTextEncoding::kUTF8, rect.fLeft + 8, rect.fTop + 128, *font, paint);

    auto color = win->pixSrc->getColor4f(x, y);
    int R{ (int)(color.fR * 255) }, G{ (int)(color.fG * 255) }, B{ (int)(color.fB * 255) };
    str = std::format("RGB(Ctrl+R): {},{},{}", R, G, B);
    data = str.data();
    canvas->drawSimpleText(data, str.size(), SkTextEncoding::kUTF8, rect.fLeft + 8, rect.fTop + 157, *font, paint);


    std::stringstream ss;
    ss << std::hex << (R << 16 | G << 8 | B);
    std::string hex = ss.str();
    size_t str_length = hex.length();
    for (size_t i = 0; i < 6 - str_length; i++) {
        hex = "0" + hex;
    }
    std::transform(hex.begin(), hex.end(), hex.begin(), toupper);
    str = std::format("HEX(Ctrl+H): #{}", hex);
    
    data = str.data();
    canvas->drawSimpleText(data, str.size(), SkTextEncoding::kUTF8, rect.fLeft + 8, rect.fTop + 186, *font, paint);

    win->Refresh();
    return false;
}

PixelInfo::PixelInfo()
{
}
