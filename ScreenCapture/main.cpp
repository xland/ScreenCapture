#include "Canvas.h"
#include <Windows.h>
#include <thread>
#include <memory>
#include <SFML/Graphics.hpp>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow){
    auto canvas = new Canvas();
    canvas->ShotScreen();
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode::getFullscreenModes()[0], "ScreenCapture", sf::Style::Fullscreen,settings);
    window.setFramerateLimit(20);
    window.setVerticalSyncEnabled(true);
    auto flag = window.setActive(false);
    std::thread t(&Canvas::Init,canvas, &window);
    t.join();
    while (window.isOpen())
    {
        sf::Event event;
        while (window.waitEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else {
                canvas->ProcessEvent(event);
            }
        }
    }
    delete canvas;
    return 0;
}
