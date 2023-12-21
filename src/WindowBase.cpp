#include "WindowBase.h"
#include <dwmapi.h>
#include "include/effects/SkRuntimeEffect.h"
#include "Util.h"


#include "modules/skparagraph/include/Paragraph.h"
#include "modules/skparagraph/include/ParagraphBuilder.h"
#include "modules/skparagraph/include/ParagraphStyle.h"
#include "modules/skparagraph/include/TextStyle.h"

WindowBase::WindowBase()
{
}

WindowBase::~WindowBase()
{
    delete[] pixBase->addr();
    delete pixBase;
    delete pixBack;
    DeleteDC(hCompatibleDC);
    DeleteObject(bottomHbitmap);
}

void WindowBase::Show()
{
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    Refresh();
    SetCursor(LoadCursor(nullptr, IDC_ARROW));
}

void WindowBase::Refresh()
{
    surfaceBack->writePixels(*pixBase, 0, 0);
    auto back = surfaceBack->getCanvas();
    auto front = surfaceFront->getCanvas();
    front->clear(SK_ColorTRANSPARENT);
    //front->saveLayer(nullptr,nullptr);
    paint(front);

    //auto fontMgr = SkFontMgr::RefDefault();
//int count = fontMgr->countFamilies();
//for (int i = 0; i < count; ++i) {
//    SkString familyName;
//    fontMgr->getFamilyName(i, &familyName);
//    SkDebugf(familyName.c_str());
//    //std::cout << "Font Family Name: " << familyName.c_str() << std::endl;
//}

    //SkPaint paint;
    //paint.setColor(SK_ColorRED);
    //paint.setStroke(false);
    //sk_sp<skia::textlayout::FontCollection> fontCollection = sk_make_sp<skia::textlayout::FontCollection>();
    //fontCollection->setDefaultFontManager(SkFontMgr::RefDefault());
    //fontCollection->enableFontFallback();
    //skia::textlayout::ParagraphStyle paraStyle;
    //auto builder = skia::textlayout::ParagraphBuilder::make(paraStyle, fontCollection);
    //skia::textlayout::TextStyle defaultStyle;
    //std::vector<SkString> ffs;
    //ffs.push_back(SkString{ "Arial" });
    //defaultStyle.setFontFamilies(ffs);
    //defaultStyle.setFontStyle(SkFontStyle(SkFontStyle::Weight::kBold_Weight, SkFontStyle::Width::kNormal_Width, SkFontStyle::Slant::kItalic_Slant));
    ////defaultStyle.setDecoration(skTextDe(TextDecoration::kNoDecoration));
    //defaultStyle.setFontSize(30);
    //defaultStyle.setForegroundColor(paint);
    //builder->pushStyle(defaultStyle);
    //builder->addPlaceholder(skia::textlayout::PlaceholderStyle());
    //std::string hello = "hello\nworld";
    //builder->addText(hello.data(), hello.size());
    //auto paragraph = builder->Build();
    //auto l = builder->getText();
    //paragraph->layout(148.f);
    //paragraph->paint(front, 0, 0);


    //auto fFontCollection = sk_make_sp<skia::textlayout::FontCollection>();
    //fFontCollection->setDefaultFontManager(SkFontMgr::RefDefault());
    //skia::textlayout::TextStyle fTStyle;
    //fTStyle.setFontFamilies({ SkString("Arial") });
    //fTStyle.setColor(SK_ColorBLACK);
    //const char* text =
    //    "This is a very long sentence to test if the text will properly wrap "
    //    "around and go to the next line. Sometimes, short sentence. Longer "
    //    "sentences are okay too because they are necessary. Very short. "
    //    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod "
    //    "tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim "
    //    "veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea "
    //    "commodo consequat. Duis aute irure dolor in reprehenderit in voluptate "
    //    "velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint "
    //    "occaecat cupidatat non proident, sunt in culpa qui officia deserunt "
    //    "mollit anim id est laborum. "
    //    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod "
    //    "tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim "
    //    "veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea "
    //    "commodo consequat. Duis aute irure dolor in reprehenderit in voluptate "
    //    "velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint "
    //    "occaecat cupidatat non proident, sunt in culpa qui officia deserunt "
    //    "mollit anim id est laborum.";
    //skia::textlayout::ParagraphStyle paragraph_style;
    //auto builder =
    //    skia::textlayout::ParagraphBuilder::make(paragraph_style, fFontCollection);
    //if (!builder) {
    //    return;
    //}
    //builder->pushStyle(fTStyle);
    //builder->addText(text,strlen(text));
    //builder->pop();
    //auto fParagraph = builder->Build();
    //fParagraph->layout(1024);
    //fParagraph->paint(front, 200, 200);

    //front->restore();
    sk_sp<SkImage> img = surfaceFront->makeImageSnapshot();
    back->drawImage(img, 0, 0);
    HDC hdc = GetDC(hwnd);
    static BITMAPINFO info = {sizeof(BITMAPINFOHEADER), w, 0 - h, 1, 32, BI_RGB, w * 4 * h, 0, 0, 0, 0};
    SetDIBits(hdc, bottomHbitmap, 0, h, pixBack->addr(), &info, DIB_RGB_COLORS);
    static BLENDFUNCTION blend = {.BlendOp{AC_SRC_OVER}, .SourceConstantAlpha{255}, .AlphaFormat{AC_SRC_ALPHA}};
    static POINT pSrc = {0, 0};
    static SIZE sizeWnd = {w, h};
    UpdateLayeredWindow(hwnd, hdc, NULL, &sizeWnd, hCompatibleDC, &pSrc, NULL, &blend, ULW_ALPHA);
    ReleaseDC(hwnd, hdc);
}

void WindowBase::initCanvas()
{
    HDC hdc = GetDC(hwnd);
    hCompatibleDC = CreateCompatibleDC(NULL);
    bottomHbitmap = CreateCompatibleBitmap(hdc, w, h);
    DeleteObject(SelectObject(hCompatibleDC, bottomHbitmap));
    ReleaseDC(hwnd, hdc);
    SkImageInfo info = SkImageInfo::MakeN32Premul(w, h);
    surfaceBack = SkSurfaces::Raster(info);
    pixBack = new SkPixmap();
    surfaceBack->peekPixels(pixBack);
    surfaceFront = SkSurfaces::Raster(info);
}

void WindowBase::Close(const int &exitCode)
{
    SendMessage(hwnd, WM_CLOSE, NULL, NULL);
    PostQuitMessage(0);
}

LRESULT CALLBACK WindowBase::RouteWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT *pCS = reinterpret_cast<CREATESTRUCT *>(lParam);
        LPVOID pThis = pCS->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    auto obj = reinterpret_cast<WindowBase *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (obj)
    {
        switch (msg)
        {
        case WM_NCCALCSIZE:
        {
            if (wParam == TRUE)
            {
                return false;
            }
            break;
        }
        case WM_REFRESH: {
            obj->Refresh();
            return true;
        }
        case WM_SETCURSOR:
        {
            return true;
        }
        case WM_KEYDOWN:
        {
            switch (wParam)
            {
            case VK_ESCAPE:
            {
                obj->Close(3);
                return false;
            }
            }
            return false;
        }
        case WM_KEYUP:
        {
            return false;
        }
        default:
        {
            return obj->WindowProc(hWnd, msg, wParam, lParam);
        }
        }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
void WindowBase::initWindow()
{
    static int num = 0;
    std::wstring className = std::format(L"ScreenCapture{}", num++);
    auto hinstance = GetModuleHandle(NULL);
    WNDCLASSEX wcx{};
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.lpfnWndProc = &WindowBase::RouteWindowMessage;
    wcx.cbWndExtra = sizeof(WindowBase *);
    wcx.hInstance = hinstance;
    wcx.hIcon = LoadIcon(hinstance, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(hinstance, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcx.lpszClassName = className.c_str();
    if (!RegisterClassEx(&wcx))
    {
        return;
    }
    auto exStyle = WS_EX_LAYERED;
    this->hwnd = CreateWindowEx(exStyle, wcx.lpszClassName, wcx.lpszClassName, WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP,
                                x, y, w, h, NULL, NULL, hinstance, static_cast<LPVOID>(this));
    BOOL attrib = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &attrib, sizeof(attrib));
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
}