#ifndef WindowsEventHandler_h__
#define WindowsEventHandler_h__

class IWindowsEventHandler
{
public:
	virtual void HandleMessage(MSG Message) = 0;
protected:
	HWND GetWindowHandle() { return WindowHandle; }
private:
	friend class FWindow;
	HWND WindowHandle;
};

#endif // WindowsEventHandler_h__
