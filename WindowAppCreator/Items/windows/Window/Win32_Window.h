#pragma once
#ifdef _WIN32
#include "../WindowsItem.h"
#include "../../Window.h"

namespace Win32
{
	class Win32_Window : public virtual Win32_Item, public virtual Window
	{
	public:
		Win32_Window();
		Win32_Window(int width, int height, const std::string& title);
		virtual ItemMessage Init() override;
		virtual ItemMessage Update() override;
		virtual void AddChild(ItemPtr ptr) override;

	public:
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		Rect& GetRect();

	public:
		MSG	  communicate;

		WNDCLASSEX	windowClass;

	private:
		LRESULT CALLBACK RealWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	};
}//Windows
#endif