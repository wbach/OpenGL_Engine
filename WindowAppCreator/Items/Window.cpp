#include "Window.h"

void Window::RegisterAction(ActionsType type, unsigned int id, std::function<void()> func)
{
	actions[type][id] = func;
}

void Window::AddChild(ItemPtr ptr)
{
	if(ptr->OnClick != nullptr)
		RegisterAction(ActionsType::ON_CLICK, ptr->id, ptr->OnClick);

	Item::AddChild(ptr);
}
