#include "Win32_Window.h"

#ifdef _WIN32

namespace WindowTools
{
	void SetWindowClass(Win32::Win32_Window& window)
	{
		window.className = window.className + std::to_string(window.id);

		window.windowClass.cbSize = sizeof(WNDCLASSEX);
		window.windowClass.style = 0;
		window.windowClass.lpfnWndProc = Win32::Win32_Window::WndProc;
		window.windowClass.cbClsExtra = 0;
		window.windowClass.cbWndExtra = 0;
		window.windowClass.hInstance = window.instance = GetModuleHandle(NULL);
		window.windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		window.windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		window.windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		window.windowClass.lpszMenuName = NULL;
		window.windowClass.lpszClassName = (LPSTR)window.className.c_str();
		window.windowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	}

	bool SetRegisterClass(const Win32::Win32_Window& window)
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

namespace Win32
{
	Win32_Window::Win32_Window()
	{
	}
	Win32_Window::Win32_Window(int width, int height, const std::string & title)
	{
		rect.width = width;
		rect.height = height;
		this->title = title;
	}
	ItemMessage Win32_Window::Init()
	{
		WindowTools::SetWindowClass(*this);
		WindowTools::SetRegisterClass(*this);
		Win32_Item::CreateItem();
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (long)this);

		ShowWindow(hwnd, SW_SHOWDEFAULT);
		UpdateWindow(hwnd);

		return ItemMessage::SUCCES;
	}

	ItemMessage Win32_Window::Update()
	{
		while (GetMessage(&communicate, NULL, 0, 0))
		{
			TranslateMessage(&communicate);
			DispatchMessage(&communicate);
		}
		return ItemMessage::SUCCES;
	}

	void Win32_Window::AddChild(ItemPtr ptr)
	{
		Win32_Item* win_item = dynamic_cast<Win32_Item*>(ptr.get());
		if (win_item == nullptr)
		{
			MessageBox(NULL, (ptr->name + " - Win32_Window::AddChild::Error item is not win32 item.").c_str(), "Error", MB_ICONEXCLAMATION);
			return;
		}
		win_item->hWndParent = hwnd;
		Window::AddChild(ptr);
	}

	LRESULT Win32_Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		Win32_Window* me = (Win32_Window*)(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (me) return me->RealWndProc(hwnd, msg, wParam, lParam);
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	Rect & Win32_Window::GetRect()
	{
		return rect;
	}

	LRESULT CALLBACK Win32_Window::RealWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CLOSE:
			for (auto& action : actions[ActionsType::ON_EXIT])
				action.second();
			DestroyWindow(hwnd);
			break;
		case WM_COMMAND:
			actions[ActionsType::ON_CLICK][wParam]();
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}

		return 0;
	}

}//Win32
#endif