#pragma once
#include "ItemsGroup.h"
#include <unordered_map>

struct Layout
{
	std::unordered_map<std::string, ItemGroup> panels;
};
