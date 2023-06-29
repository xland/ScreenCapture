#include "Canvas.h"

Canvas::Canvas()
{

}
Canvas::~Canvas()
{

}
void Canvas::ShotScreen()
{
    //todo mac
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    DeleteObject(SelectObject(hDC, hBitmap));
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    unsigned int dataSize = ((w * 32 + 31) / 32) * 4 * h;
    BITMAPINFO Info = { sizeof(BITMAPINFOHEADER), static_cast<long>(w), static_cast<long>(0 - h), 1, 32, BI_RGB, dataSize, 0, 0, 0, 0 };
    std::uint8_t* pixels = new std::uint8_t[dataSize];
    int r = GetDIBits(hDC, hBitmap, 0, h, (LPVOID)pixels, &Info, DIB_RGB_COLORS);
    for (int x = 0; x < dataSize; x += 4)
    {
        auto r = pixels[x + 2];
        auto b = pixels[x];
        pixels[x] = r;
        pixels[x + 2] = b;
    }
    auto flag = bgTexture.create(sf::Vector2u(w, h));
    bgTexture.update(pixels);
    bg = std::make_unique<sf::Sprite>(bgTexture);
    delete[] pixels;
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);
}
void Canvas::Init(sf::RenderWindow* window)
{
    this->window = window;
    auto flag = window->setActive(true);
    Paint();
}
void Canvas::ProcessEvent(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Button::Right) 
        {
            window->close();
        }
        else
        {
            mouseDownPos.x = event.mouseButton.x;
            mouseDownPos.y = event.mouseButton.y;
            isMouseDown = true;
        }
    }
    else if (event.type == sf::Event::MouseMoved) 
    {
        if (!isMouseDown) return;
        sf::Vector2i pos;
        pos.x = event.mouseButton.x;
        pos.y = event.mouseButton.y;
        if (pos.x < mouseDownPos.x) {
            masks[0].width = pos.x;
            masks[1].left = pos.x;
            masks[1].width = mouseDownPos.x - pos.x;
            masks[2].left = mouseDownPos.x;
            masks[2].width = w - mouseDownPos.x;
            masks[3].left = mouseDownPos.x;
            masks[3].width = w - mouseDownPos.x;
            masks[4].left = mouseDownPos.x;
            masks[4].width = w - mouseDownPos.x;
            masks[5].left = pos.x;
            masks[5].width = mouseDownPos.x - pos.x;
            masks[6].width = pos.x;
            masks[7].width = pos.x;
            cutRect.left = pos.x;
            cutRect.width = mouseDownPos.x - pos.x;
        }
        else
        {
            masks[0].width = mouseDownPos.x;
            masks[1].left = mouseDownPos.x;
            masks[1].width = pos.x - mouseDownPos.x;
            masks[2].left = pos.x;
            masks[2].width = w - pos.x;
            masks[3].left = pos.x;
            masks[3].width = w - pos.x;
            masks[4].left = pos.x;
            masks[4].width = w - pos.x;
            masks[5].left = mouseDownPos.x;
            masks[5].width = pos.x - mouseDownPos.x;
            masks[6].width = mouseDownPos.x;
            masks[7].width = mouseDownPos.x;
            cutRect.left = mouseDownPos.x;
            cutRect.width = pos.x - mouseDownPos.x;
        }
        if (pos.y < mouseDownPos.y) {
            masks[0].height = pos.y;
            masks[1].height = pos.y;
            masks[2].height = pos.y;
            masks[3].top = pos.y;
            masks[3].height = mouseDownPos.y - pos.y;
            masks[4].top = mouseDownPos.y;
            masks[4].height = h - mouseDownPos.y;
            masks[5].top = mouseDownPos.y;
            masks[5].height = h - mouseDownPos.y;
            masks[6].top = mouseDownPos.y;
            masks[6].height = h - mouseDownPos.y;
            masks[7].top = pos.y;
            masks[7].height = mouseDownPos.y - pos.y;
            cutRect.top = pos.y;
            cutRect.height = mouseDownPos.y - pos.y;
        }
        else
        {
            masks[0].height = mouseDownPos.y;
            masks[1].height = mouseDownPos.y;
            masks[2].height = mouseDownPos.y;
            masks[3].top = mouseDownPos.y;
            masks[3].height = pos.y - mouseDownPos.y;
            masks[4].top = pos.y;
            masks[4].height = h - pos.y;
            masks[5].top = pos.y;
            masks[5].height = h - pos.y;
            masks[6].top = pos.y;
            masks[6].height = h - pos.y;
            masks[7].top = mouseDownPos.y;
            masks[7].height = pos.y - mouseDownPos.y;
            cutRect.top = mouseDownPos.y;
            cutRect.height = pos.y - mouseDownPos.y;
        }
        Paint();
    }
    else if (event.type == sf::Event::MouseButtonReleased)
    {
        isMouseDown = false;
    }
}
void Canvas::Paint()
{
    window->draw(*bg.get());
    for (size_t i = 0; i < 8; i++)
    {
        sf::RectangleShape shape;
        shape.setFillColor({ 0, 0, 0, 160 });
        shape.setPosition({ (float)masks[i].left, (float)masks[i].top});
        shape.setSize({ (float)masks[i].width, (float)masks[i].height});
        window->draw(shape);
    }
    window->display();
}