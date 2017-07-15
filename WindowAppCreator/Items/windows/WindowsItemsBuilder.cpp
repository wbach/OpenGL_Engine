#include "WindowsItemsBuilder.h"
#include "Window/Window.h"

ItemPtr WidowsItemBuidler::CreateSingleWindow()
{
	auto ptr = CreateItemPtr<Windows::Window>();
	ptr->Init();
	return ptr;
}
