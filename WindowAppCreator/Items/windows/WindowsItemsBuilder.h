#pragma once
#include "../IItemBuidler.h"

struct WidowsItemBuidler : public IItemBuidler
{
	virtual ItemPtr CreateSingleWindow() override;
};
