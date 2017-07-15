#include "Window.h"
#include "..\..\Window.h"
#ifdef _WIN32

namespace WindowTools
{
	void SetWindowClass(Windows::Window& window)
	{
		window.windowClass.cbSize = sizeof(WNDCLASSEX);
		window.windowClass.style = 0;
		window.windowClass.lpfnWndProc = Windows::Window::WndProc;
		window.windowClass.cbClsExtra = 0;
		window.windowClass.cbWndExtra = 0;
		window.windowClass.hInstance = window.instance = GetModuleHandle(NULL);
		window.windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		window.windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		window.windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		window.windowClass.lpszMenuName = NULL;
		window.windowClass.lpszClassName = window.className = window.windowClassName;
		window.windowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	}

	bool SetRegisterClass(const Windows::Window& window)
	{
		if (!RegisterClassEx(&window.windowClass))
		{
			MessageBox(NULL, "Wysoka Komisja odmawia rejestracji tego okna!", "Niestety...",
				MB_ICONEXCLAMATION | MB_OK);
			return false;
		}
		return true;
	}
}//WindowTools

namespace Windows
{
	void Window::Init()
	{
		WindowTools::SetWindowClass(*this);
		WindowTools::SetRegisterClass(*this);
		WindowsItem::CreateItem();

		ShowWindow(hwnd, SW_SHOWDEFAULT);
		UpdateWindow(hwnd);
	}

	LRESULT Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		Window* me = (Window*)(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (me) return me->RealWndProc(hwnd, msg, wParam, lParam);
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	Rect & Window::GetRect()
	{
		return rect;
	}

	LRESULT CALLBACK Window::RealWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}

		return 0;
	}
}//Windows
#endif

void Window::RegisterAction(ActionsType type, unsigned int id, std::function<void()> func)
{
	OnCreateFunctions[type][id] = func;
}
