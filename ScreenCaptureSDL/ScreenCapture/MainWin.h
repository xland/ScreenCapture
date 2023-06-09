#pragma once
#include <Windows.h>
#include <memory>
#include <SDL.h>
#include "State.h"
class MainWin
{
public:
	MainWin();
	~MainWin();
	void Start();
public:
private:
	void initWindowGeomtry();
	void initWindow();
	void shotScreen();
	void render();
	void createMaskByMouse(const SDL_Point& pos);
private:
	SDL_Window* win;
	SDL_Surface* bgImg;
	SDL_Surface* screenSurface;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	SDL_Rect maskRect[8];
	SDL_Rect cutRect;
	SDL_Rect screenRect;
	bool isMouseDown = false;
	SDL_Point mouseDownPos;
	State state = State::Start;
};

