#pragma once
#ifdef _WIN32
#include "../Item.h"
#include <Windows.h>

namespace Windows
{
	struct WindowsItem : public Item
	{
		virtual void Init() = 0;

		HWND hwnd;

		LPSTR className;
		HINSTANCE instance;

		bool CreateItem()
		{
			hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, className, title.c_str(), WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, CW_USEDEFAULT, 240, 120, NULL, NULL, instance, NULL);

			if (hwnd == NULL)
			{
				MessageBox(NULL, (name + "::Error creatind windows item.").c_str(), "Error", MB_ICONEXCLAMATION);
				return false;
			}
			return true;
		}
	};
}
#endif