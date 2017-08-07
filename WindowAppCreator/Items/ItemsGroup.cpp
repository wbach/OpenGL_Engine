#include "ItemsGroup.h"

void ItemGroup::Assign(ItemPtr item)
{
	items.push_back(item);
}

void ItemGroup::SetPosition(int x, int y)
{
	rect.x = x;
	rect.y = y;

	for (auto& item : items)
	{
		item->rect.x += x;
		item->rect.y += y;
	}

}
