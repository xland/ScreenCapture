注意：需要把skia输出目录下的icudtl.dat拷贝到exe所在目录

```c++
#include <Windows.h>
#include "WindowMain.h"
#include "include/core/SkGraphics.h"
#include "modules/skparagraph/include/Paragraph.h"
#include "modules/skparagraph/include/ParagraphBuilder.h"
#include "modules/skparagraph/include/ParagraphStyle.h"
#include "modules/skparagraph/include/TextStyle.h"
#include "include/core/SkStream.h"
#include "include/encode/SkPngEncoder.h"

void draw(SkCanvas* canvas) {
    sk_sp<skia::textlayout::FontCollection> fontCollection = sk_make_sp<skia::textlayout::FontCollection>();
    fontCollection->setDefaultFontManager(SkFontMgr::RefDefault());
    fontCollection->enableFontFallback();
    skia::textlayout::ParagraphStyle paraStyle;
    auto builder = skia::textlayout::ParagraphBuilder::make(paraStyle, fontCollection);
    skia::textlayout::TextStyle defaultStyle;
    defaultStyle.setFontFamilies({ SkString{"Times New Roman"} });
    defaultStyle.setColor(SK_ColorBLACK);
    //defaultStyle.setFontStyle(SkFontStyle(SkFontStyle::Weight::kBold_Weight, SkFontStyle::Width::kNormal_Width, SkFontStyle::Slant::kItalic_Slant));
    //defaultStyle.setDecoration(skTextDe(TextDecoration::kNoDecoration));
    defaultStyle.setFontSize(18);
    builder->pushStyle(defaultStyle);
    const char* hello = "test test test";
    builder->addText(hello, strlen(hello));
    builder->pop();
    auto paragraph = builder->Build();
    paragraph->layout(18.f);
    paragraph->paint(canvas, 0, 0);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
    SkGraphics::Init();
    SkImageInfo info = SkImageInfo::MakeN32Premul(800, 600);
    auto surface = SkSurfaces::Raster(info);
    draw(surface->getCanvas());
    sk_sp<SkImage> image = surface->makeImageSnapshot();
    SkFILEWStream stream("output123.bmp");
    SkPixmap pixmap;
    image->peekPixels(&pixmap);
    SkPngEncoder::Options option;
    SkPngEncoder::Encode(&stream, pixmap, option);
    stream.flush();
    return 0;
}
```