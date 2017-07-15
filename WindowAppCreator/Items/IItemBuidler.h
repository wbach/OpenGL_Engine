#pragma once
#include "Item.h"

struct IItemBuidler
{
	virtual ~IItemBuidler() {}
	virtual ItemPtr CreateSingleWindow() = 0;
};
