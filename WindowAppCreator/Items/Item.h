#pragma once
#include "Rect.h"
#include <string>
#include <memory>

typedef std::shared_ptr<Item> ItemPtr;

template<typename T, typename... Args>
ItemPtr CreateItemPtr(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

struct Item
{
	Item()
		: name("default name")
		, title("default title")
		, id(0)
	{
	}
	virtual ~Item() {};
	virtual void Init() = 0;
	Rect rect;
	std::string name;
	std::string title;
	unsigned int id;	
};
