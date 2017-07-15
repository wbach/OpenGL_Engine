#pragma once
#include "Rect.h"
#include "ItemMessages.h"
#include "Actions.h"

#include <list>
#include <string>
#include <memory>
#include <functional>

struct Item;

typedef std::shared_ptr<Item> ItemPtr;

template<typename T, typename... Args>
ItemPtr CreateItemPtr(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

struct Item
{
	Item();
	virtual ~Item() {};
	virtual ItemMessage Init() = 0;
	virtual ItemMessage Update() { return{}; }
	virtual void AddChild(ItemPtr);
	std::function<void()> OnClick = nullptr;

	Rect rect;
	std::string name;
	std::string title;

	std::list<ItemPtr> children;

	unsigned int id;
	static unsigned int s_Id;
};
