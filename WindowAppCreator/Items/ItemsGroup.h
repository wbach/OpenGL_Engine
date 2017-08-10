#pragma once
#include "Item.h"
#include "Rect.h"
#include <list>

struct ItemGroup
{
public:
	void Assign(ItemPtr item);
	void SetPosition(int x, int y);

private:
	Rect rect;
	std::list<ItemPtr> items;
};
