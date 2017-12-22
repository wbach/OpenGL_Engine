#include "XMLUtils.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include <sstream>

namespace Utils
{
	rapidNodeData GetRapidNodeData(rapidxml::xml_node<char>* node)
	{
		return{node->name(), node->value()};
	}

	rapidNodeData GetRapidAttributeData(rapidxml::xml_attribute<char>* node)
	{
		return{ node->name(), node->value() };
	}

	void AddVectorToNode(rapidxml::xml_document<char>& document, rapidxml::xml_node<char>* node, const glm::vec3 vector)
	{
		rapidxml::xml_node<char>* x = document.allocate_node(rapidxml::node_element, "x", document.allocate_string(std::to_string(vector.x).c_str()));
		rapidxml::xml_node<char>* y = document.allocate_node(rapidxml::node_element, "y", document.allocate_string((std::to_string(vector.y).c_str())));
		rapidxml::xml_node<char>* z = document.allocate_node(rapidxml::node_element, "z", document.allocate_string((std::to_string(vector.z).c_str())));
		node->append_node(x);
		node->append_node(y);
		node->append_node(z);
	}

	void AddVectorToNode(rapidxml::xml_document<char>& document, rapidxml::xml_node<char>* node, const glm::vec2 vector)
	{
		rapidxml::xml_node<char>* x = document.allocate_node(rapidxml::node_element, "x", document.allocate_string(std::to_string(vector.x).c_str()));
		rapidxml::xml_node<char>* y = document.allocate_node(rapidxml::node_element, "y", document.allocate_string((std::to_string(vector.y).c_str())));
		node->append_node(x);
		node->append_node(y);
	}

	std::string MessageBuilder(std::multimap<std::string, std::string>& messeges)
	{
		rapidxml::xml_document<char> document;

		rapidxml::xml_node<char>* node = document.allocate_node(rapidxml::node_element, "msg");

		for (const auto& pair : messeges)
		{
			rapidxml::xml_node<char>* subnode = document.allocate_node(rapidxml::node_element, document.allocate_string(pair.first.c_str()), document.allocate_string(pair.second.c_str()));
			node->append_node(subnode);
		}

		document.append_node(node);

		std::stringstream ss;
		//ss << document;
		document.clear();
		return ss.str();
	}
}