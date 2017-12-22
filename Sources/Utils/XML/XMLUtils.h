#pragma once
#include <map>
#include <glm/glm.hpp>

namespace rapidxml
{
	template<class T> class xml_node;
	template<class T> class xml_attribute;
	template<class T> class xml_document;
}

namespace Utils
{
	struct rapidNodeData
	{
		std::string name;
		std::string value;
	};

	rapidNodeData GetRapidNodeData(rapidxml::xml_node<char>* node);
	rapidNodeData GetRapidAttributeData(rapidxml::xml_attribute<char>* node);
	void AddVectorToNode(rapidxml::xml_document <char>& document, rapidxml::xml_node<char>* node, const glm::vec3 vector);
	void AddVectorToNode(rapidxml::xml_document <char>& document, rapidxml::xml_node<char>* node, const glm::vec2 vector);
	std::string MessageBuilder(std::multimap<std::string, std::string>& messeges);
}