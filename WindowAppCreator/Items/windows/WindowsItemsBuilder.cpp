#include "WindowsItemsBuilder.h"
#include "Window/Win32_Window.h"
#include "Button/Win32_Button.h"

ItemPtr WidowsItemBuidler::CreateWindowItem()
{
	auto ptr = CreateItemPtr<Win32::Win32_Window>();
	return ptr;
}
ItemPtr WidowsItemBuidler::CreateWindowItem(int width, int height, const std::string& title)
{
	auto ptr = CreateItemPtr<Win32::Win32_Window>(width, height, title);
	return ptr;
}

ItemPtr WidowsItemBuidler::CreateButton(int x, int y, int w, int h, const std::string& t, std::function<void()> f)
{
	auto ptr = CreateItemPtr<Win32::Win32_Button>(x, y, w, h, t, f);
	return ptr;
}

