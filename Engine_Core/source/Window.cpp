#include "Window.h"
#include "Input.h"
#include <algorithm>

static LRESULT CALLBACK MyWindowProc(_In_ HWND   hwnd, _In_ UINT   uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	if (uMsg == WM_QUIT || uMsg == WM_CLOSE)
	{
		PostQuitMessage(0);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

FWindow::FWindow(HINSTANCE hInstance, int nCmdShow, int Width, int Height) : WindowWidth(Width), WindowHeight(Height)
{
	RegisterWindow(hInstance);

	WindowHandle = CreateWindow ("WindowClass",
		"Engine Window",
		WS_OVERLAPPED | WS_THICKFRAME | WS_MAXIMIZEBOX,
		0,
		0,
		Width,
		Height,
		NULL,
		NULL,
		hInstance,
		NULL);

	SetWindowLong(WindowHandle, GWL_USERDATA, (LONG)this);

	ShowWindow(WindowHandle, nCmdShow);
	UpdateWindow(WindowHandle);
}

FWindow::~FWindow()
{
	DestroyWindow(WindowHandle);
}

bool FWindow::HandleEvents()
{
	bool IsWindowBeingDestroyed = false;

	MSG Message;
	while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Message);

		if (Message.message == WM_QUIT || Message.message == WM_CLOSE || Message.message == WM_DESTROY)
		{
			IsWindowBeingDestroyed = true;
		}

		for (auto EventHandler : EventHandlers)
		{
			EventHandler->HandleMessage(Message);
		}

		DispatchMessage(&Message);
	}

	return IsWindowBeingDestroyed == false;
}

// Defines the window
void FWindow::RegisterWindow(HINSTANCE hInstance)
{
	WNDCLASSEX  wcex;

	wcex.cbSize        = sizeof (wcex);
	wcex.style         = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = MyWindowProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = hInstance;
	wcex.hIcon         = 0; 
	wcex.hCursor       = LoadCursor (NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
	wcex.lpszMenuName  = NULL;
	wcex.lpszClassName = "WindowClass";
	wcex.hIconSm       = 0;

	RegisterClassEx (&wcex);
}

int FWindow::GetWidth() const
{
	RECT wRect;
	GetWindowRect(WindowHandle, &wRect);
	return wRect.right - wRect.left;
}

int FWindow::GetHeight() const
{
	RECT wRect;
	GetWindowRect(WindowHandle, &wRect);
	return wRect.bottom - wRect.top;
}

void FWindow::SetTitle(const std::string& Title)
{
	SetWindowText(WindowHandle, Title.c_str());
}

void FWindow::RegisterEventHandler(IWindowsEventHandler* EventHandler)
{
	EventHandlers.Add(EventHandler);
	EventHandler->WindowHandle = WindowHandle;
}

void FWindow::UnregisterEventHandler(IWindowsEventHandler* EventHandler)
{
	EventHandler->WindowHandle = nullptr;
	EventHandlers.Remove(EventHandler);
}

void FMessageBox::ShowSimpleMessage(LPSTR MessageText)
{
	MessageBox(NULL, MessageText, "MessageBox", MB_OK);
}
