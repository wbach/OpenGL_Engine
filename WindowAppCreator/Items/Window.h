#pragma once
#include "./Item.h"
#include <functional>
#include <unordered_map>
#include <list>

typedef std::unordered_map<unsigned int, std::function<void()>> FunctionsMap;

enum ActionsType
{
	ON_CREATE,
	ON_EXIT,
	ON_PRESS,
	ON_TOUCH,
	COUNT
};

class Window : public Item
{	
public:
	virtual void Create() = 0;
	void RegisterAction(ActionsType, unsigned int, std::function<void()>);

protected:
	FunctionsMap OnCreateFunctions[ActionsType::COUNT];
	std::list<ItemPtr> children;
};