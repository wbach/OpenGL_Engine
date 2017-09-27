#pragma once
#include "../IItemBuidler.h"

struct WidowsItemBuidler : public IItemBuidler
{
	virtual ItemPtr CreateWindowItem() override;
	virtual ItemPtr CreateWindowItem(int, int, const std::string&, ActionsMap = ActionsMap()) override;
	virtual ItemPtr CreateChildWindowItem(int, int, const std::string&) override;
	virtual ItemPtr CreateOpenGLChildWindowItem(int, int, int, int, const std::string&) override;
	virtual ItemPtr CreateButton(int, int, int, int, const std::string&, std::function<void()>) override;
};
