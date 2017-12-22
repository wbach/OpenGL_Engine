#include "WindowsItemsBuilder.h"
#include "Window/Win32_Window.h"
#include "Button/Win32_Button.h"

ItemPtr WidowsItemBuidler::CreateWindowItem()
{
	auto ptr = CreateItemPtr<Win32::Win32_Window>();
	return ptr;
}
ItemPtr WidowsItemBuidler::CreateWindowItem(int width, int height, const std::string& title, std::multimap<ActionsType, std::function<void()>> functions)
{
	int id = 0;

	auto window = new Win32::Win32_Window(width, height, title);
	for (const auto& f : functions)
	{
		window->RegisterAction(f.first, ++id, f.second);
	}
	return CreateItemPtr<Win32::Win32_Window>(window);
}

ItemPtr WidowsItemBuidler::CreateChildWindowItem(int width, int height, const std::string& title)
{
	auto raw_ptr = new Win32::Win32_Window(width, height, title);
	//raw_ptr->dwExStyle = 0;
	raw_ptr->dwStyle |= WS_CHILD;
	
	auto ptr = CreateItemPtrFromRawPtr<Win32::Win32_Window>(raw_ptr);
	return ptr;
}

ItemPtr WidowsItemBuidler::CreateOpenGLChildWindowItem(int x, int y, int width, int height, const std::string& title)
{
	auto raw_ptr = new Win32::Win32_Window(width, height, title);
	raw_ptr->windowType = Win32::WindowType::OPENGL;
	raw_ptr->windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	raw_ptr->windowClass.hbrBackground = NULL;
	raw_ptr->dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	raw_ptr->dwStyle = WS_OVERLAPPEDWINDOW;

	auto ptr = CreateItemPtrFromRawPtr<Win32::Win32_Window>(raw_ptr);
	ptr->rect.x = x;
	ptr->rect.y = y;

	return ptr;
}

ItemPtr WidowsItemBuidler::CreateButton(int x, int y, int w, int h, const std::string& t, std::function<void()> f)
{
	auto ptr = CreateItemPtr<Win32::Win32_Button>(x, y, w, h, t, f);
	return ptr;
}

