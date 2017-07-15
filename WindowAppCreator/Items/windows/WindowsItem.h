#pragma once
#ifdef _WIN32
#include "../Item.h"
#include <Windows.h>

namespace Win32
{
	struct Win32_Item : public virtual Item
	{
		HWND hwnd;

		DWORD dwExStyle = WS_EX_CLIENTEDGE;
		DWORD dwStyle = WS_OVERLAPPEDWINDOW;
		HWND hWndParent = NULL;
		HMENU hMenu = NULL;

		LPVOID lpParam = NULL;
		std::string className = "default class name";
		HINSTANCE instance;

		bool CreateItem()
		{
			hwnd = CreateWindowEx(dwExStyle, className.c_str(), title.c_str(), dwStyle,
				rect.x, rect.y, rect.width, rect.height, hWndParent, hMenu, instance, lpParam);

			if (hwnd == NULL)
			{
				MessageBox(NULL, (name + "::Error creatind windows item.").c_str(), "Error", MB_ICONEXCLAMATION);
				return false;
			}
			
			HFONT hNormalFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hwnd, WM_SETFONT, (WPARAM) hNormalFont, 0);

			return true;
		}
	};
}
#endif

//virtual void Init() = 0;
//virtual ~Win32_Item() {}
//
//HWND hwnd;
//
//DWORD dwExStyle = WS_EX_CLIENTEDGE;
//LPCSTR lpClassName;
//LPCSTR lpWindowName = "";
//DWORD dwStyle = WS_OVERLAPPEDWINDOW;
//int X = CW_USEDEFAULT;
//int Y = CW_USEDEFAULT;
//int nWidth = 320;
//int nHeight = 240;
//HWND hWndParent = NULL;
//HMENU hMenu = NULL;
//HINSTANCE hInstance = NULL;
//LPVOID lpParam = NULL;
//
//bool CreateItem()
//{
//	hwnd = CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle,
//		X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
//
//	if (hwnd == NULL)
//	{
//		MessageBox(NULL, (lpWindowName + "::Error creatind windows item.").c_str(), "Error", MB_ICONEXCLAMATION);
//		return false;
//	}
//	return true;
//}