#pragma once
#include "Item.h"
#include "Window.h"
#include <map>

typedef std::multimap<ActionsType, std::function<void()>> ActionsMap;

struct IItemBuidler
{
	virtual ~IItemBuidler() {}
	//CreateWindow with default params
	virtual ItemPtr CreateWindowItem() = 0;
	//width, height, title
	virtual ItemPtr CreateWindowItem(int, int, const std::string&, ActionsMap = ActionsMap()) = 0;
	//width, height, title
	virtual ItemPtr CreateChildWindowItem(int, int, const std::string&) = 0;
	//pos x, pos y, width, height, title
	virtual ItemPtr CreateOpenGLChildWindowItem(int, int, int, int, const std::string&) = 0;
	//pos x, pos y, width, height, text, func
	virtual ItemPtr CreateButton(int, int, int, int, const std::string&, std::function<void ()>) = 0;
};
