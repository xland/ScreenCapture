#pragma once
#include <Windows.h>
#include "include/core/SkSurface.h"
#include "src/utils/win/SkWGL.h"
#include <GL/gl.h>
#include "src/gpu/gl/GrGLUtil.h"
class MainWin
{
public:
	MainWin(HINSTANCE hinstance);
	~MainWin();
	LRESULT CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
private:
	void initWindowGeomtry();
	void shotVirtualScreen();
	void createNativeWindow();
	void initSurface();
	sk_sp<SkSurface> getSurface();
	void disposeSurfaceResource();
	void paint();

	HWND hwnd;
	HINSTANCE hinstance;
	bool isActive = false;
	int stencilBits{ 0 };
	HGLRC hglrc{ nullptr };
	const GrGLInterface* backendContext{ nullptr };
	GrDirectContext* directContext{ nullptr };
	int x, y, width, height;
	sk_sp<SkImage> bgImg;
};
namespace {
	MainWin* win;
}

