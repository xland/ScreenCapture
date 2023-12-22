#include "ShapeMosaic.h"
#include "../WindowMain.h"
#include "../ToolSub.h"
#include "include/core/SkMaskFilter.h"
#include "include/core/SkBlurTypes.h"
#include "include/effects/SkBlurMaskFilter.h"

ShapeMosaic::ShapeMosaic(const int &x, const int &y) : ShapeBase(x, y)
{
    initParams();
}

ShapeMosaic::~ShapeMosaic()
{
}

bool ShapeMosaic::OnPaint(SkCanvas *canvas)
{
    //SkImageInfo info = SkImageInfo::MakeN32Premul(800, 600);
    //auto surface = SkSurfaces::Raster(info);
    //auto canvasTemp = surface->getCanvas();
    //canvasTemp->clear(SK_ColorTRANSPARENT);
    //SkPaint paintTemp;
    //paintTemp.setStrokeWidth(60);
    //paintTemp.setStrokeCap(SkPaint::Cap::kRound_Cap);
    //paintTemp.setStrokeJoin(SkPaint::kRound_Join);
    //paintTemp.setStroke(true);
    //SkPath pathTemp;
    //pathTemp.moveTo(0, 0);
    //pathTemp.lineTo(800, 600);
    //canvas->drawPath(pathTemp, paintTemp);
    //sk_sp<SkImage> img = surface->makeImageSnapshot();


    SkPaint paint;
    auto maskFilter = SkMaskFilter::MakeBlur(SkBlurStyle::kSolid_SkBlurStyle,10.f,false);
    //sk_sp<SkMaskFilter> mosaicMaskFilter = SkMaskFilter::MakeBlur(SkBlurStyle::kNormal_SkBlurStyle, 10.f);
    paint.setAntiAlias(true);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    paint.setStrokeJoin(SkPaint::kRound_Join);
    paint.setStroke(true);
    paint.setStrokeWidth(strokeWidth);
    paint.setMaskFilter(maskFilter);
    SkSamplingOptions options;
    //canvas->drawImage(img,0,0, options, &paint);
    canvas->drawPath(path, paint);
    return false;
}

bool ShapeMosaic::OnMouseDown(const int &x, const int &y)
{
    return false;
}

bool ShapeMosaic::OnMouseUp(const int &x, const int &y)
{
    return false;
}

bool ShapeMosaic::OnMouseMove(const int &x, const int &y)
{
    return false;
}

bool ShapeMosaic::OnMoseDrag(const int &x, const int &y)
{
    IsWIP = false;
    path.lineTo(x, y);
    WindowMain::get()->Refresh();
    return false;
}

void ShapeMosaic::initParams()
{
    HoverIndex = 4;
    path.moveTo(startX, startY);
    auto tool = ToolSub::get();
    stroke = !tool->getFill();
    if (stroke)
    {
        auto stroke = tool->getStroke();
        if (stroke == 1)
        {
            strokeWidth = 26;
        }
        else if (stroke == 2)
        {
            strokeWidth = 56;
        }
        else
        {
            strokeWidth = 86;
        }
    }
}