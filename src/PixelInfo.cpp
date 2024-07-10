#include "PixelInfo.h"
#include "App.h"
#include "WindowBase.h"
#include "include/core/SkRegion.h"
#include "include/core/SkColor.h"
#include "string"
#include "format"
#include <sstream>
#include <algorithm>
#include "Lang.h"

PixelInfo* pixelInfo;

PixelInfo::~PixelInfo()
{
}

void PixelInfo::Init()
{
    if (!pixelInfo)
    {
        pixelInfo = new PixelInfo();
    }
}

PixelInfo* PixelInfo::Get()
{
    return pixelInfo;
}

bool PixelInfo::OnMouseMove(const int& x, const int& y) {
    auto win = App::GetWin();
    if (win->IsMouseDown || win->state >= State::mask)
    {
        return false;
    }
    win->Refresh();
}

bool PixelInfo::OnPaint(SkCanvas* canvas)
{
    auto win = App::GetWin();
    if (win->IsMouseDown || win->state >= State::mask)
    {
        return false;
    }
    float width{ 200.0f },height{ 200.0f },x,y;
    {
        POINT mousePos;
        GetCursorPos(&mousePos);
        x = (float)mousePos.x;
        y = (float)mousePos.y;
    }
    rect.setXYWH(x + 10, y + 10, width, height);    
    if (rect.fLeft + width > win->w) {
        rect.offset(0 - width - 20, 0);
    }
    if (rect.fTop + height > win->h) {
        rect.offset(0, 0 - height - 20);
    }
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

    auto str = std::format(L"{}: X:{}  Y:{}", Lang::Get(Lang::Key::Position), x, y);
    auto data = str.data();
    canvas->drawSimpleText(data, str.size()*2, SkTextEncoding::kUTF16, rect.fLeft + 8, rect.fTop + 128, *font, paint);

    auto color = win->pixSrc->getColor4f(x, y);
    int R{ (int)(color.fR * 255) }, G{ (int)(color.fG * 255) }, B{ (int)(color.fB * 255) };
    str = std::format(L"RGB(Ctrl+R): {},{},{}", R, G, B);
    data = str.data();
    canvas->drawSimpleText(data, str.size()*2, SkTextEncoding::kUTF16, rect.fLeft + 8, rect.fTop + 157, *font, paint);


    std::wstringstream ss;
    ss << std::hex << (R << 16 | G << 8 | B);
    std::wstring hex = ss.str();
    size_t str_length = hex.length();
    for (size_t i = 0; i < 6 - str_length; i++) {
        hex = L"0" + hex;
    }
    std::transform(hex.begin(), hex.end(), hex.begin(), toupper);
    str = std::format(L"HEX(Ctrl+H): #{}", hex);
    
    data = str.data();
    canvas->drawSimpleText(data, str.size()*2, SkTextEncoding::kUTF16, rect.fLeft + 8, rect.fTop + 186, *font, paint);
    return false;
}

PixelInfo::PixelInfo()
{
}
