#include "MainWin.h"
#include "spdlog/spdlog.h"
#include "SDL_opengl.h"

namespace {
    void log(const std::string&& message) {
#ifdef _DEBUG
        OutputDebugStringA(message.c_str());
        OutputDebugStringA("\r\n");
#else
        spdlog::get("logger")->info(message);
#endif
    }
}


MainWin::MainWin()
{
    initWindowGeomtry();
    shotScreen();
    initWindow();

}
MainWin::~MainWin()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_FreeSurface(bgImg);
    SDL_DestroyWindow(win);    
}
void MainWin::Start()
{    
    SDL_Event e;
    bool quit = false;
    while (quit == false)
    {
        while (SDL_WaitEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT) {
                quit = true;
                break;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                isMouseDown = true;
                SDL_GetMouseState(&mouseDownPos.x, &mouseDownPos.y);
            }
            else if (e.type == SDL_MOUSEMOTION) {
                if (!isMouseDown) continue;
                SDL_Point pos;
                SDL_GetMouseState(&pos.x,&pos.y);
                if (state == State::Start) {
                    createMaskByMouse(pos);
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP) {
                isMouseDown = false;
                if (state == State::Start) {
                    state = State::MaskReady;
                }
            }
        }
    }
}


void MainWin::initWindowGeomtry()
{
    screenRect.x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    screenRect.y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    screenRect.w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    screenRect.h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    maskRect[0].x = 0;
    maskRect[0].y = 0;
    maskRect[0].w = screenRect.w;
    maskRect[0].h = screenRect.h;
}
void MainWin::render()
{
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x88);
    for (size_t i = 0; i < 8; i++)
    {
        SDL_RenderFillRect(renderer, &maskRect[i]);
    }
    SDL_SetRenderDrawColor(renderer, 0x22, 0xAA, 0x99, 0xFF);

    glColor4ub(255, 0, 0, 255);
    glLineWidth(8.0f);
    glBegin(GL_LINES);
    glVertex2i(cutRect.x, cutRect.y);
    glVertex2i(cutRect.x+ cutRect.w, cutRect.y+ cutRect.h);
    glEnd();

    SDL_RenderDrawRect(renderer, &cutRect);
    SDL_RenderPresent(renderer);
}
void MainWin::initWindow()
{
    win = SDL_CreateWindow("Hello World", screenRect.x, screenRect.y, screenRect.w, screenRect.h, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
    if (win == nullptr)
    {
        log("initWindow error");
        return;
    }
    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    texture = SDL_CreateTextureFromSurface(renderer, bgImg);    
    render();
    //screenSurface = SDL_GetWindowSurface(win);
    //SDL_BlitSurface(bgImg, NULL, screenSurface, NULL);
    //SDL_UpdateWindowSurface(win);

    
}
void  MainWin::shotScreen()
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, screenRect.w, screenRect.h);
    BOOL result = DeleteObject(SelectObject(hDC, hBitmap));
    if (!result) {
        log("DeleteObject error");
    }
    result = BitBlt(hDC, 0, 0, screenRect.w, screenRect.h, hScreen, screenRect.x, screenRect.y, SRCCOPY);
    if (!result) {
        log("BitBlt error");
    }
    unsigned int dataSize = ((screenRect.w * 32 + 31) / 32) * 4 * screenRect.h;
    BITMAPINFO Info = { sizeof(BITMAPINFOHEADER), static_cast<long>(screenRect.w), static_cast<long>(0- screenRect.h), 1, 32, BI_RGB, dataSize, 0, 0, 0, 0 };
    bgImg = SDL_CreateRGBSurface(SDL_SWSURFACE, screenRect.w, screenRect.h, 32, 0, 0, 0, 0);
    SDL_LockSurface(bgImg);
    int r = GetDIBits(hDC, hBitmap, 0, screenRect.h, bgImg->pixels, &Info, DIB_RGB_COLORS);
    SDL_UnlockSurface(bgImg);
    if (r == 0) {
        log("GetDIBits error");
    }
}