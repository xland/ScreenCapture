#include "MainWin.h"
#include "include/core/SkCanvas.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "src/core/SkMathPriv.h"
#include "src/gpu/GrCaps.h"
#include "src/gpu/GrDirectContextPriv.h"
#include "src/gpu/gl/GrGLDefines.h"
#include "src/image/SkImage_Base.h"
#include "src/utils/win/SkWGL.h"
#include <GL/gl.h>
#include "src/gpu/gl/GrGLUtil.h"
#include "DisplayParams.h"

static DisplayParams displayParam;
void MainWin::initSurface()
{
    HDC dc = GetDC(hwnd);
    hglrc = SkCreateWGLContext(dc, displayParam.fMSAASampleCount, false, kGLPreferCompatibilityProfile_SkWGLContextRequest);
    if (nullptr == hglrc) {
        MessageBox(nullptr, L"SkCreateWGLContext error", L"info", MB_OK);
        return;
    }
    SkWGLExtensions extensions;
    if (extensions.hasExtension(dc, "WGL_EXT_swap_control")) {
        extensions.swapInterval(displayParam.fDisableVsync ? 0 : 1);
    }
    if (wglMakeCurrent(dc, hglrc)) {
        glClearStencil(0);
        glClearColor(0, 0, 0, 0);
        glStencilMask(0xffffffff);
        glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        int pixelFormat = GetPixelFormat(dc);
        PIXELFORMATDESCRIPTOR pfd;
        DescribePixelFormat(dc, pixelFormat, sizeof(pfd), &pfd);
        stencilBits = pfd.cStencilBits;
        MessageBox(nullptr, L"wglMakeCurrent ok", L"info", MB_OK);
    }
    auto param = GrGLMakeNativeInterface();
    backendContext = param.release();
    auto context = GrDirectContext::MakeGL(param, displayParam.fGrContextOptions);
    directContext = context.release();
    if (!backendContext) {
        MessageBox(nullptr, L"GrGLMakeNativeInterface error", L"info", MB_OK);
    }
    MessageBox(nullptr, L"initSurface ok", L"info", MB_OK);
}

sk_sp<SkSurface> MainWin::getSurface()
{
    glViewport(0, 0,width, height);
    GrGLint buffer;
    GR_GL_CALL(backendContext, GetIntegerv(GR_GL_FRAMEBUFFER_BINDING, &buffer));
    GrGLFramebufferInfo fbInfo;
    fbInfo.fFBOID = buffer;
    fbInfo.fFormat = GR_GL_RGBA8;
    GrBackendRenderTarget backendRT(width, height, displayParam.fMSAASampleCount, stencilBits, fbInfo);

    MessageBox(nullptr, L"no error now", L"info", MB_OK);

    auto fSurface = SkSurface::MakeFromBackendRenderTarget(directContext, backendRT,
        kBottomLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType,
        displayParam.fColorSpace,
        &displayParam.fSurfaceProps);

    if (fSurface.get()) {
        MessageBox(nullptr, L"getSurface ok", L"info", MB_OK);
    }
    else
    {
        MessageBox(nullptr, L"getSurface error", L"info", MB_OK);
    }
    return fSurface;
}
void MainWin::disposeSurfaceResource()
{
    if (directContext) {
        directContext->abandonContext();
        delete directContext;
    }
    delete backendContext;
    wglDeleteContext(hglrc);
    hglrc = nullptr;
}