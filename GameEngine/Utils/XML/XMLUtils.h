#pragma once
#include "rapidxml-1.13/rapidxml.hpp"
#include <map>
#include <glm/glm.hpp>

namespace Utils
{
	void AddVectorToNode(rapidxml::xml_document <>& document, rapidxml::xml_node<>* node, const glm::vec3 vector);
	void AddVectorToNode(rapidxml::xml_document <>& document, rapidxml::xml_node<>* node, const glm::vec2 vector);
	std::string MessageBuilder(std::multimap<std::string, std::string>& messeges);
}