#include "Item.h"

unsigned int Item::s_Id = 1000;

Item::Item()
	: name("default name")
	, title("default title")
	, id(s_Id++)
{
}

void Item::AddChild(ItemPtr ptr)
{
	children.push_back(ptr);
}