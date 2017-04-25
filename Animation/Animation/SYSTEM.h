/*#ifndef SYSTEM_H
#define SYSTEM_H

#include <Windows.h>
#include "GRAPHICS.h"
#include "INPUT.h"
#include "OPENGL.h"

static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 600;
static const bool FULL_SCREEN = false;

class System
{
public:
	System();
	~System();

	bool Init();
	void Shutdown();
	void MainLoop();
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool F
	bool InitWindows();
	void ShutdownWindows();

private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	LPCWSTR m_sAppName;
	int m_iWidth;
	int m_iHeight;

	OpenGL* m_pOpenGL;
	Graphics* m_pGraphics;
	Input* m_pInput;
};

static LRESULT CALLBACK MainMessageHandler(HWND, UINT, WPARAM, LPARAM);
static System* g_pAppHandler = 0;

#endif //SYSTEM_H
*/