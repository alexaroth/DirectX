#include "graphics.h"

bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed);
int messageloop();
LRESULT CALLBACK WndProc(HWND hWnd,	UINT msg, WPARAM wParam, LPARAM lParam);
void gameLoopDelay();

const int Width  = 1200;
const int Height = 900;
float frameTime = 0, tick = 0;
HWND hwnd = nullptr;
LPCTSTR WndClassName = L"firstwindow";
graphics graphicsMain(Width, Height);



//Main windows function
int WINAPI WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{ 
	if(!InitializeWindow(hInstance, nShowCmd, Width, Height, true))
	{
		MessageBox(0, L"Window Initialization failed", L"Error", MB_OK);
		return 0;
	}


    if(!graphicsMain.InitDirectInput(hInstance, hwnd))
	{
		MessageBox(0, L"Direct Input Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

    if(!graphicsMain.InitializeDirect3d11App(Width, Height, hwnd))	//Initialize Direct3D
	{
		MessageBox(0, L"Direct3D Initialization failed", L"Error", MB_OK);
		return 0;
	}

	if(!graphicsMain.InitScene())	//Initialize our scene
	{
		MessageBox(0, L"Scene Initialization failed", L"Error", MB_OK);
		return 0;
	}

	messageloop();

	graphicsMain.CleanUp();    

	return 0;
}

bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed)
{
	typedef struct _WNDCLASS 
    {
		UINT cbSize;
		UINT style;
		WNDPROC lpfnWndProc;
		int cbClsExtra;
		int cbWndExtra;
		HANDLE hInstance;
		HICON hIcon;
		HCURSOR hCursor;
		HBRUSH hbrBackground;
		LPCTSTR lpszMenuName;
		LPCTSTR lpszClassName;
	} WNDCLASS;

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = WndClassName;
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(nullptr, L"Error registering class", L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	hwnd = CreateWindowEx(NULL, WndClassName, L"Shooting Range Sim", WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, CW_USEDEFAULT,
		                  width, height, nullptr, nullptr, hInstance, nullptr);

	if (!hwnd)
	{
		MessageBox(nullptr, L"Error creating window", L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	ShowWindow(hwnd, ShowWnd);
	UpdateWindow(hwnd);

	return true;
}

int messageloop()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while(true)
	{
		BOOL PeekMessageL(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
           
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		}

		else
        {
            tick = GetTickCount();
            graphicsMain.detectInput();
			graphicsMain.UpdateScene(frameTime);
			graphicsMain.DrawScene();                             
            frameTime = GetTickCount() - tick;
            gameLoopDelay();
		}

      
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{

	case WM_KEYDOWN:

		if( wParam == VK_ESCAPE )
        {
			DestroyWindow(hwnd);
		}

		return 0;

	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void gameLoopDelay()
{
  if (frameTime < 15)
	  {
          //Sleep(1);
	      Sleep(15 - (int)frameTime);
	  }
}