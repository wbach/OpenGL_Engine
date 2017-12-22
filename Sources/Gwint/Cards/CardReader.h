#pragma once
#include "Card.h"
#include <list>

namespace rapidxml
{
	template<class Ch> class xml_node;
}

namespace CardReader
{	
	std::vector<Skills> ParseSkills(rapidxml::xml_node<char> * node);
	SCard ParseCard(rapidxml::xml_node<char> * node);
	std::vector<SCard> ReadCardFromString(const std::string& xml_str);
	std::vector<SCard> ReadCardFromXML(const std::string& file_name);
}