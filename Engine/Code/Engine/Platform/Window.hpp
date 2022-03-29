#pragma once
#include <string>

class InputSystem;
struct Vec2;
struct IntVec2;
class Window
{
public:
	Window();
	~Window();
public:
	void* m_hwnd = nullptr;
	bool Open(std::string const& title, float aspectRatio = 2.f, float ratioOfHeight = .9f /*0.9f*/);
	void Close();
	void BeginFrame();
	void EndFrame();
	bool wasJustActivated();
	bool wasJustDeactivated();
	unsigned int GetClientWidth();
	unsigned int GetClientHeight();
	void SetInputSystem(InputSystem* input);
	void InputToClipBoard(std::string text);
	std::string GetFromClipBoard();
	IntVec2 GetWindowCenter();
	InputSystem* m_input = nullptr;
	
	bool m_isQuitting = false;
	bool m_isActivated = false;
	bool m_wasActivated = false;
private:
	unsigned int m_clientHeight = 0;
	unsigned int m_clientWidth = 0;
};