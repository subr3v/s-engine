#include "Input.h"
#include <iostream>

#include <windows.h>
#include <gl/GL.h>
#include "glm/gtc/matrix_transform.hpp"

#include "MemoryUtils.h"

FInput::FInput()
{
	ResetArray(bMouseDown);
	ResetArray(bMousePressed);
	ResetArray(bKeysDown);
	ResetArray(bKeysPressed);
}

FInput::~FInput()
{

}

void FInput::SetMousePosition(int X, int Y)
{
	SetCursorPos(X, Y);
}

void FInput::SignalFrameEnded()
{
	// Reset pressed arrays to 0
	ResetArray(bKeysPressed);
	ResetArray(bMousePressed);
}

void FInput::CenterMouseScreen(class FWindow* Window)
{
	POINT WindowCenter;
	WindowCenter.x = Window->GetWidth() / 2;
	WindowCenter.y = Window->GetHeight() / 2;
	ClientToScreen(Window->GetHandle(), &WindowCenter);

	SetMousePosition(WindowCenter.x, WindowCenter.y);
}

void FInput::HandleMouseButtonDown(MouseButton Button)
{
	if (bMouseDown[(int)Button] == false)
	{
		bMouseDown[(int)Button] = bMousePressed[(int)Button] = true;
	}
}

void FInput::HandleMouseButtonUp(MouseButton Button)
{
	bMouseDown[(int)Button] = bMousePressed[(int)Button] = false;
}

void FInput::HandleMessage(MSG Message)
{
	char MessageKey = Message.wParam;
	switch (Message.message)
	{
	case WM_KEYDOWN:
		if (bKeysDown[MessageKey] == false)
		{
			bKeysDown[MessageKey] = bKeysPressed[MessageKey] = true;
		}
		break;
	case WM_KEYUP:
		bKeysDown[MessageKey] = bKeysPressed[MessageKey] = false;
		break;
	case WM_RBUTTONDOWN:
		HandleMouseButtonDown(MouseButton::Right);
		break;
	case WM_MBUTTONDOWN:
		HandleMouseButtonDown(MouseButton::Middle);
		break;
	case WM_LBUTTONDOWN:
		HandleMouseButtonDown(MouseButton::Left);
		break;
	case WM_RBUTTONUP:
		HandleMouseButtonUp(MouseButton::Right);
		break;
	case WM_MBUTTONUP:
		HandleMouseButtonUp(MouseButton::Middle);
		break;
	case WM_LBUTTONUP:
		HandleMouseButtonUp(MouseButton::Left);
		break;
	case WM_MOUSEMOVE:
		MouseX = LOWORD(Message.lParam);
		MouseY = HIWORD(Message.lParam);
		break;
	case WM_CHAR:
		if (CurrentTextCount < kMaxCharTextBuffer && Message.wParam > 0 && Message.wParam < 0x10000)
		{
			InputTextBuffer[CurrentTextCount++] = (u8)Message.wParam;
		}
	default:
		break;
	}
}

int FInput::ConsumeInputTextBuffer(u8*& TextBuffer)
{
	TextBuffer = InputTextBuffer;
	int CharCount = CurrentTextCount;
	CurrentTextCount = 0;
	return CharCount;
}
