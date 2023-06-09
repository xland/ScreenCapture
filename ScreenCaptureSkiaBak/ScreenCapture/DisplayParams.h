#pragma once
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurfaceProps.h"
#include "include/gpu/GrContextOptions.h"
class DisplayParams {
public:
    DisplayParams()
        : colorType(kN32_SkColorType)
        , fColorSpace(nullptr) 
        , fMSAASampleCount(8)  //重要
        , fSurfaceProps(0, kRGB_H_SkPixelGeometry)
        , fDisableVsync(false)
        , fDelayDrawableAcquisition(false)
        , fEnableBinaryArchive(false)
    {}
    /// <summary>
    /// 颜色类型，默认为BGRA，有点奇怪
    /// </summary>
    SkColorType         colorType;
    /// <summary>
    /// 颜色空间，不知道干啥的，共享指针
    /// </summary>
    sk_sp<SkColorSpace> fColorSpace;
    /// <summary>
    /// 采样
    /// </summary>
    int                 fMSAASampleCount;
    GrContextOptions    fGrContextOptions;
    SkSurfaceProps      fSurfaceProps;
    bool                fDisableVsync;
    bool                fDelayDrawableAcquisition;
    bool                fEnableBinaryArchive;
};