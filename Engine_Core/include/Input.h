#ifndef INPUT_H
#define INPUT_H

#include <Windows.h>
#include "Window.h"
#include <glm/glm.hpp>
#include "WindowsEventHandler.h"

enum class MouseButton : u8
{
	Left,
	Right,
	Middle,
	NUMBER_OF_BUTTONS,
};

class FInput : public IWindowsEventHandler
{
public:
	FInput();
	~FInput();

	bool IsKeyDown(int Key) const
	{
		return bKeysDown[Key];
	}

	bool IsKeyPressed(int Key) const
	{
		return bKeysPressed[Key];
	}

	bool IsMouseButtonDown(MouseButton Button) const
	{
		return bMouseDown[(int)Button];
	}

	bool IsMouseButtonPressed(MouseButton Button) const
	{
		return bMousePressed[(int)Button];
	}

	void SignalFrameEnded();

	glm::vec2 GetMousePosition() const { return glm::vec2(MouseX, MouseY); }
	void SetMousePosition(int X, int Y);
	void CenterMouseScreen(class FWindow* Window);

	const bool* GetLowLevelKeysDown() const { return bKeysDown; }
	const bool* GetLowLevelMouseDown() const { return bMouseDown; }

	int ConsumeInputTextBuffer(u8*& TextBuffer);
private:
	friend class FWindow;

	void HandleMouseButtonDown(MouseButton Button);
	void HandleMouseButtonUp(MouseButton Button);

	virtual void HandleMessage(MSG Message) override;

	static const int kMaxCharTextBuffer = 256;

	int MouseX = 0, MouseY = 0;
	bool bKeysDown[256];
	bool bKeysPressed[256];
	bool bMouseDown[(u32)MouseButton::NUMBER_OF_BUTTONS];
	bool bMousePressed[(u32)MouseButton::NUMBER_OF_BUTTONS];
	u8 InputTextBuffer[kMaxCharTextBuffer];
	int CurrentTextCount = 0;
};

#endif // INPUT_H