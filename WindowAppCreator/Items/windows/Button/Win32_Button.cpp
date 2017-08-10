#include "Win32_Button.h"

Win32::Win32_Button::Win32_Button()
	: Win32_Button(0, 0, 50, 25, "Button", nullptr)
{

}

Win32::Win32_Button::Win32_Button(int x, int y, int width, int height, const std::string & text, std::function<void()> onclick)
{
	rect.x = x;
	rect.y = y;
	rect.width = width;
	rect.height = height;
	title = text;
	OnClick = onclick;
	SetLocalVaribles();
}

ItemMessage Win32::Win32_Button::Init()
{
	Win32_Item::CreateItem();
	return ItemMessage();
}

ItemMessage Win32::Win32_Button::Update()
{
	return ItemMessage();
}

void Win32::Win32_Button::SetLocalVaribles()
{
	dwExStyle = 0;
	dwStyle = WS_CHILD | WS_VISIBLE;
	className = "BUTTON";
	hMenu = (HMENU) id;
}
