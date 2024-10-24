#include <qlayout.h>
#include <qpushbutton.h>
#include <QEvent>

#include "App.h"
#include "WinBoard.h"
#include "WinCanvas.h"
#include "WinMask.h"
#include "WinFull.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"
#include "Shape/ShapeDragger.h"
#include "Shape/ShapeBase.h"
#include "Shape/ShapeRect.h"
#include "Shape/ShapeEllipse.h"

WinBoard::WinBoard(QWidget* parent) : QWidget(parent)
{
    initImgs();  
}

WinBoard::~WinBoard()
{
}

void WinBoard::dispatchEvent(QGraphicsSceneHoverEvent* e)
{
    // shapeRect->hoverMoveEvent(e);
}


void WinBoard::paintEvent(QPaintEvent* event)
{

    QPainter painter(this);
    // painter.drawImage(rect(), imgBg);
    painter.drawPixmap(rect(), desktopImg);
    //auto& shapes = App::getFull()->shapes;
    //for (size_t i = 0; i < shapes.size(); i++)
    //{
    //    shapes[i]->paint(&painter);
    //}
}

void WinBoard::initImgs()
{
    auto screenRect = QRect(GetSystemMetrics(SM_XVIRTUALSCREEN), GetSystemMetrics(SM_YVIRTUALSCREEN),
            GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
    auto screens = QGuiApplication::screens();
    for (auto screen : screens)
    {
        auto pos = screen->geometry().topLeft();
        if (pos.x() == 0 && pos.y() == 0)
        {
            auto dpr = screen->devicePixelRatio();
            desktopImg = screen->grabWindow(0,
                    screenRect.x() / dpr,
                    screenRect.y() / dpr,
                    screenRect.width() / dpr,
                    screenRect.height() / dpr);
            break;
        }
    }

    //auto winNative = WinFull::Get();
    //HDC hScreen = GetDC(NULL);
    //HDC hDC = CreateCompatibleDC(hScreen);
    //HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, winNative->w, winNative->h);
    //DeleteObject(SelectObject(hDC, hBitmap));
    //BOOL bRet = BitBlt(hDC, 0, 0, winNative->w, winNative->h, hScreen, winNative->x, winNative->y, SRCCOPY);
    //long long dataSize = winNative->w * winNative->h * 4;
    //std::vector<unsigned char> bgPix(dataSize, 0);
    //BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)winNative->w, 0 - (long)winNative->h, 1, 32, BI_RGB, (DWORD)dataSize, 0, 0, 0, 0 };
    //GetDIBits(hDC, hBitmap, 0, winNative->h, &bgPix.front(), &info, DIB_RGB_COLORS);
    //DeleteObject(hBitmap);
    //DeleteDC(hDC);
    //ReleaseDC(NULL, hScreen);
    //QImage bgTemp(&bgPix.front(), winNative->w, winNative->h, QImage::Format_ARGB32);
    ////auto bg = bgTemp.convertToFormat(QImage::Format_RGB444); //QImage::Format_RGB444 让图像体积小一倍，但图像颜色会变得不一样
    ////imgBg = std::make_unique<QImage>(std::move(bgTemp));
    //imgBg = std::make_unique<QImage>(bgTemp.copy(0, 0, winNative->w, winNative->h));

}
