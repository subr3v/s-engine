#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>
#include <stdio.h>
#include <string>
#include "Vector.h"
#include "WindowsEventHandler.h"

class FMessageBox
{
public:
	static void ShowSimpleMessage(LPSTR MessageText);
};

class FWindow
{
public:
	FWindow(HINSTANCE hInstance, int nCmdShow, int Width, int Height);
	~FWindow();

	bool HandleEvents();

	void RegisterEventHandler(IWindowsEventHandler* EventHandler);
	void UnregisterEventHandler(IWindowsEventHandler* EventHandler);

	HWND GetHandle() { return WindowHandle; }
	int GetWidth() const;
	int GetHeight() const;

	void SetTitle(const std::string& Title);
private:
	void RegisterWindow(HINSTANCE);

	HWND WindowHandle;
	int WindowWidth, WindowHeight;
	TVector<IWindowsEventHandler*> EventHandlers;
};

#endif // WINDOW_H