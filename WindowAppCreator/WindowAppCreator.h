#pragma once
#include "Items/IItemBuidler.h"
#include <unordered_map>

typedef std::unordered_map<std::string, ItemPtr> ItemsMap;
typedef std::unique_ptr<IItemBuidler> ItemBuilderPtr;

template<typename T, typename... Args>
ItemBuilderPtr CreateItemBuilderPtr(Args&&... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

struct WindowAppCreator
{
	WindowAppCreator();

	ItemBuilderPtr itemBuilder;
	ItemsMap items;
};