#include "XMLUtils.h"
//#include "rapidxml-1.13/rapidxml_print.hpp"
#include <sstream>

void Utils::AddVectorToNode(rapidxml::xml_document<>& document, rapidxml::xml_node<>* node, const glm::vec3 vector)
{
	rapidxml::xml_node<>* x = document.allocate_node(rapidxml::node_element, "x", document.allocate_string(std::to_string(vector.x).c_str()));
	rapidxml::xml_node<>* y = document.allocate_node(rapidxml::node_element, "y", document.allocate_string((std::to_string(vector.y).c_str())));
	rapidxml::xml_node<>* z = document.allocate_node(rapidxml::node_element, "z", document.allocate_string((std::to_string(vector.z).c_str())));
	node->append_node(x);
	node->append_node(y);
	node->append_node(z);
}

void Utils::AddVectorToNode(rapidxml::xml_document<>& document, rapidxml::xml_node<>* node, const glm::vec2 vector)
{
	rapidxml::xml_node<>* x = document.allocate_node(rapidxml::node_element, "x", document.allocate_string(std::to_string(vector.x).c_str()));
	rapidxml::xml_node<>* y = document.allocate_node(rapidxml::node_element, "y", document.allocate_string((std::to_string(vector.y).c_str())));
	node->append_node(x);
	node->append_node(y);
}

std::string Utils::MessageBuilder(std::multimap<std::string, std::string>& messeges)
{
	rapidxml::xml_document<> document;

	rapidxml::xml_node<>* node = document.allocate_node(rapidxml::node_element, "msg");

	for (const auto& pair : messeges)
	{
		rapidxml::xml_node<>* subnode = document.allocate_node(rapidxml::node_element, document.allocate_string(pair.first.c_str()), document.allocate_string(pair.second.c_str()));
		node->append_node(subnode);
	}

	document.append_node(node);

	std::stringstream ss;
    //ss << document;
	document.clear();
	return ss.str();
}
