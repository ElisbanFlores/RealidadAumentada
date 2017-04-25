/*#include "SYSTEM.h"

System::System()
{
	m_pOpenGL = NULL;
	m_pGraphics = NULL;
	m_pInput = NULL;
}

System::~System()
{

}

bool System::Init()
{
	m_pOpenGL = new OpenGL();
	if(!m_pOpenGL)
		return false;

	if(!InitWindows())
		return false;

	if(!m_pOpenGL->Init(m_hWnd))
		return false;

	//INPUT
	m_pInput = new Input();
	if(!m_pInput)
		return false;

	if(!m_pInput->Init())
		return false;

	//GRAPHICS
	m_pGraphics = new Graphics();
	if(!m_pGraphics)
		return false;

	if(!m_pGraphics->Init(m_pOpenGL))
		return false;


	return true;
}

void System::Shutdown()
{
	if(m_pGraphics)
	{
		m_pGraphics->Shutdown();
		delete m_pGraphics;
		m_pGraphics = NULL;
	}
	if(m_pInput)
	{
		delete m_pInput;
		m_pInput = NULL; 
	}
	if(m_pOpenGL)
	{
		m_pOpenGL->Shutdown(m_hWnd);
		delete m_pOpenGL;
		m_pOpenGL = NULL;
	}
	ShutdownWindows();
}

void System::MainLoop()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	bool bDone = false;
	
	while(!bDone)
	{
		if(msg.message == WM_QUIT)
		{
			bDone = true;
		}

		if(PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if(!Frame())
				bDone = true;
		}
	}
}

LRESULT CALLBACK System::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_KEYDOWN:
		m_pInput->KeyDown((unsigned int)wParam);
		break;
	case WM_KEYUP:
		m_pInput->KeyUp((unsigned int)wParam);
		break;
	case WM_SIZE:
		m_pOpenGL->OnResize(LOWORD(lParam), HIWORD(lParam));
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

bool System::Frame()
{
	if(m_pInput->IsKeyDown(VK_ESCAPE))
		return false;

	if(!m_pGraphics->Frame())
		return false;

	return true;
}

bool System::InitWindows()
{
	g_pAppHandler = this;
	m_hInstance = GetModuleHandle(NULL);
	m_sAppName = L"OpenGL Engine";

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbClsExtra = 0;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hInstance = m_hInstance;
	wc.lpfnWndProc = MainMessageHandler;
	wc.lpszClassName = m_sAppName;
	wc.lpszMenuName = 0;
	wc.style = CS_HREDRAW | CS_VREDRAW  | CS_OWNDC;

	if(!RegisterClassEx(&wc))
		return false;

	//
	m_hWnd = CreateWindow(m_sAppName, m_sAppName, WS_POPUP, 0, 0, 640, 480, 0, 0, m_hInstance, 0);
	if(!m_hWnd)
		return false;

	ShowWindow(m_hWnd, SW_HIDE);

	if(!m_pOpenGL->InitExtensions(m_hWnd))
		return false;

	DestroyWindow(m_hWnd);
	m_hWnd = NULL;

	int posX, posY;
	DWORD style;
	if(FULL_SCREEN)
	{
		posX = 0;
		posY = 0;
		m_iWidth = GetSystemMetrics(SM_CXSCREEN);
		m_iHeight = GetSystemMetrics(SM_CYSCREEN);
		style = WS_POPUP;
	}
	else
	{
		style = WS_OVERLAPPEDWINDOW;
		RECT rc = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
		AdjustWindowRect(&rc, style, 0);
		m_iWidth = rc.right - rc.left;
		m_iHeight = rc.bottom - rc.top;
		posX = (GetSystemMetrics(SM_CXSCREEN) - m_iWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - m_iHeight) / 2;
	}

	m_hWnd = CreateWindow(m_sAppName, m_sAppName, style, posX, posY, 
						  m_iWidth, m_iHeight, 0, 0, m_hInstance, 0);
	if(!m_hWnd)
		return false;

	ShowWindow(m_hWnd, SW_SHOW);

	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	return true;
}

void System::ShutdownWindows()
{
	DestroyWindow(m_hWnd);
	m_hWnd = NULL;

	UnregisterClass(m_sAppName, m_hInstance);
	m_hInstance = NULL;

	g_pAppHandler = NULL;
}

LRESULT CALLBACK MainMessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		return g_pAppHandler->MessageHandler(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}*/