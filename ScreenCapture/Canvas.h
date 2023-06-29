#pragma once
#include <Windows.h>
#include <thread>
#include <memory>
#include <SFML/Graphics.hpp>
class Canvas
{
public:
	Canvas();
	~Canvas();
	void ShotScreen();
	void Init(sf::RenderWindow* window);
	void SetWindowGemory();
	void Paint();
	void ProcessEvent(const sf::Event& event);
private:
	int x, y, w, h;
	sf::Texture bgTexture;
	std::unique_ptr<sf::Sprite> bg;
	sf::RenderWindow* window;
	sf::IntRect masks[8];
	sf::IntRect cutRect;
	sf::Vector2i mouseDownPos;
	bool isMouseDown;
};

