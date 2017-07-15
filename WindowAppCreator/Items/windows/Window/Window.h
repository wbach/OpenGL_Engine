#pragma once
#ifdef _WIN32
#include "../WindowsItem.h"

namespace Windows
{
	class Window : public WindowsItem
	{
	public:
		virtual void Init() override;
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		Rect& GetRect();

	public:
		MSG	  communicate;

		LPSTR		windowClassName = "default class name";
		WNDCLASSEX	windowClass;

	private:
		LRESULT CALLBACK RealWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	};
}//Windows
#endif