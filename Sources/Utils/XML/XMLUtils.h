#pragma once
#include <functional>
#include "Glm.h"
#include <map>
#include "Types.h"

namespace rapidxml
{
template <class T>
class xml_node;
template <class T>
class xml_attribute;
template <class T>
class xml_document;
}  // namespace rapidxml

namespace Utils
{
struct RapidNodeData
{
    std::string name;
    std::string value;
};
RapidNodeData GetRapidNodeData(rapidxml::xml_node<char>* node);
RapidNodeData GetRapidAttributeData(rapidxml::xml_node<char>* node, const std::string& atttributeName);
RapidNodeData GetRapidAttributeData(rapidxml::xml_attribute<char>* node);
void ForEachSubNode(rapidxml::xml_node<char>* parent,
                    std::function<void(const RapidNodeData&, rapidxml::xml_node<char>*)> func);
void ForEachAttribute(rapidxml::xml_attribute<char>* att,
                      std::function<void(const RapidNodeData&, rapidxml::xml_attribute<char>*)> func);
void AddVectorToNode(rapidxml::xml_document<char>& document, rapidxml::xml_node<char>* node, const glm::vec3 vector);
void AddVectorToNode(rapidxml::xml_document<char>& document, rapidxml::xml_node<char>* node, const glm::vec2 vector);
std::string MessageBuilder(std::multimap<std::string, std::string>& messeges);
bool CheckXmlObjectType(const std::string& file, const std::string& type);
}  // namespace Utils
