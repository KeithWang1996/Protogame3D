#include "Engine/Platform/Window.hpp"
#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/IntVec2.hpp"
//#include "ThirdParty/imgui/imgui_impl_win32.h"

//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static TCHAR const* WND_CLASS_NAME = TEXT("Simple Window Class");

static LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
	//if (ImGui_ImplWin32_WndProcHandler(windowHandle, wmMessageCode, wParam, lParam))
	//{
	//	return true;
	//}
	Window* window = (Window*)::GetWindowLongPtr(windowHandle, GWLP_USERDATA);
	switch (wmMessageCode)
	{
		case WM_ACTIVATE:
		{
			if (wParam > 0)
			{
				window->m_isActivated = true;
			}
			else
			{
				window->m_isActivated = false;
			}
			break;
		}
		case WM_CHAR:
		{
			wchar_t character = (wchar_t)wParam;
			// g_theDevConsole->OnKeyPress( character ); 
			window->m_input->PushCharacter(static_cast<char>(character));
			break;

		}
		
		case WM_LBUTTONDOWN:
		{
			//unsigned char asKey = (unsigned char)wParam;
			window->m_input->UpdateKey(KEY_MOUSE_LEFT, true);
			return 0;
			break;
		}

		case WM_LBUTTONUP:
		{
			//unsigned char asKey = (unsigned char)wParam;
			window->m_input->UpdateKey(KEY_MOUSE_LEFT, false);
			return 0;
			break;
		}

		case WM_RBUTTONDOWN:
		{
			window->m_input->UpdateKey(KEY_MOUSE_RIGHT, true);
			return 0;
			break;
		}

		case WM_RBUTTONUP:
		{
			window->m_input->UpdateKey(KEY_MOUSE_RIGHT, false);
			return 0;
			break;
		}

		case WM_MOUSEWHEEL:
		{
			//DWORD scrollFixedPoint = wParam >> 16; // shift away low word part, leaving only the high word
			float scrollAmount = (float)(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA); // convert to a numeric value
			window->m_input->AddMouseWheelScrollAmount(scrollAmount);
			return 0;
			break;
		}
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
		case WM_CLOSE:
		{
			window->m_isQuitting = true;
			return 0;
			 // "Consumes" this message (tells Windows "okay, we handled it")
		}

		// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
		case WM_KEYDOWN:
		{
			unsigned char asKey = (unsigned char)wParam;
			//if( g_theApp->HandleKeyPressed( asKey ) )
				//return 0;
			window->m_input->UpdateKey(asKey, true);//should return a bool
			return 0;
			// #SD1ToDo: Tell the App and InputSystem about this key-pressed event...

			//if( asKey == VK_ESCAPE ) // #SD1ToDo: move this "check for ESC pressed" code to App
			//{
			//	g_isReadyToQuit = true;
			//	return 0; // "Consumes" this message (tells Windows "okay, we handled it")
			//}
			break;
		}

		// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
		case WM_KEYUP:
		{
			unsigned char asKey = (unsigned char)wParam;
			//if( g_theApp->HandleKeyReleased( asKey ))

			window->m_input->UpdateKey(asKey, false);
			return 0;
			//			#SD1ToDo: Tell the App and InputSystem about this key-released event...
			break;
		}
	}

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}

static void RegisterWindowClass()
{
	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset(&windowClassDescription, 0, sizeof(windowClassDescription));
	windowClassDescription.cbSize = sizeof(windowClassDescription);
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle(NULL);
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT("Simple Window Class");
	RegisterClassEx(&windowClassDescription);
}

static void UnregisterWindowClass()
{
	::UnregisterClass(WND_CLASS_NAME, GetModuleHandle(NULL));
}

Window::Window()
{
	RegisterWindowClass();
}

Window::~Window()
{
	UnregisterWindowClass();
}

bool Window::Open(std::string const& title, float clientAspect, float maxClientFractionOfDesktop /*0.9f*/ )
{

	// #SD1ToDo: Add support for full screen mode (requires different window style flags than windowed mode)
	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);
	float desktopWidth = (float)(desktopRect.right - desktopRect.left);
	float desktopHeight = (float)(desktopRect.bottom - desktopRect.top);
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	//constexpr float maxClientFractionOfDesktop = 0.90f;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if (clientAspect > desktopAspect)
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * clientAspect;
	}
	m_clientHeight = (int)clientHeight;
	m_clientWidth = (int)clientWidth;
	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int)clientMarginX;
	clientRect.right = clientRect.left + (int)clientWidth;
	clientRect.top = (int)clientMarginY;
	clientRect.bottom = clientRect.top + (int)clientHeight;

	// Calculate the outer dimensions of the physical window, including frame.
	RECT windowRect = clientRect;
	AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

	WCHAR windowTitle[1024];
	MultiByteToWideChar(GetACP(), 0, title.c_str(), -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
	HWND hwnd = CreateWindowEx(
		windowStyleExFlags,
		WND_CLASS_NAME,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		(HINSTANCE) ::GetModuleHandle(NULL),
		NULL);

	::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);

	if (hwnd == nullptr)
	{
		return false;
	}
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(cursor);
	
	m_hwnd = (void*)hwnd;

	//ImGui_ImplWin32_Init(hwnd);

	return true;
}

void Window::Close()
{
	HWND hwnd = (HWND)m_hwnd;
	if (NULL == hwnd)
	{
		return;
	}
	::DestroyWindow(hwnd);
	m_hwnd = nullptr;
	//ImGui_ImplWin32_Shutdown();
}

void Window::BeginFrame()
{
	MSG queuedMessage;
	for (;;)
	{
		const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
		if (!wasMessagePresent)
		{
			break;
		}

		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
	}
	//ImGui_ImplWin32_NewFrame();
}

void Window::EndFrame()
{
	m_wasActivated = m_isActivated;
}

bool Window::wasJustActivated()
{
	return !m_wasActivated && m_isActivated;
}

bool Window::wasJustDeactivated()
{
	return m_wasActivated && !m_isActivated;
}

unsigned int Window::GetClientWidth()
{
	return m_clientWidth;
}
unsigned int Window::GetClientHeight()
{
	return m_clientHeight;
}

void Window::SetInputSystem(InputSystem* input)
{
	m_input = input;
	input->m_window = this;
}

void Window::InputToClipBoard(std::string text)
{
	if (text.size() <= 0)
	{
		return;
	}
	HWND hwnd = (HWND)m_hwnd;
	if (!OpenClipboard((HWND)(hwnd)))
		return;
	EmptyClipboard();
	int textLength = (int)text.size();
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE,
		(textLength + 1) * sizeof(TCHAR));
	if (hglbCopy == NULL)
	{
		CloseClipboard();
		return;
	}
	LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy);
	memcpy(lptstrCopy, &text[0],
		textLength * sizeof(TCHAR));
	lptstrCopy[textLength] = (TCHAR)0;

	GlobalUnlock(hglbCopy);
	SetClipboardData(CF_TEXT, hglbCopy);
	CloseClipboard();
}

std::string Window::GetFromClipBoard()
{
	HWND hwnd = (HWND)m_hwnd;
	if (!OpenClipboard((HWND)(hwnd)))
		return "";
	HGLOBAL hglb = GetClipboardData(CF_TEXT);
	if (hglb == NULL)
	{
		CloseClipboard();
		return "";
	}
	GlobalLock(hglb);
	
	std::string text = static_cast<char*>(GlobalLock(hglb));
	GlobalUnlock(hglb);
	CloseClipboard();
	return text;
}

IntVec2 Window::GetWindowCenter()
{
	RECT clientRect;
	GetWindowRect((HWND)m_hwnd, &clientRect);
	return IntVec2((clientRect.left + clientRect.right) / 2,
		(clientRect.top + clientRect.bottom) / 2);
}