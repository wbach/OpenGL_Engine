#pragma once
#include <functional>
#include "Glm.h"
#include <map>
#include "Types.h"
#include "XmlNode.h"

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

float ReadFloat(XmlNode& node);
bool ReadBool(XmlNode& node);
vec3 ReadVec3(XmlNode& node);
vec4 ReadVec4(XmlNode& node);
vec2ui ReadVec2ui(XmlNode& node);
vec2 ReadVec2(XmlNode& node);
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
std::unique_ptr<XmlNode> Convert(const std::string& label, const vec2&);
std::unique_ptr<XmlNode> Convert(const std::string& label, const vec3&);
vec2 ConvertToVec2(XmlNode&);
vec3 ConvertToVec3(XmlNode&);

template <class T>
std::unique_ptr<XmlNode> Convert(const std::string& label, const std::vector<T>& v)
{
    auto root = std::make_unique<XmlNode>(label);

    for (const auto& element : v)
    {
        root->AddChild(std::move(Convert(element)));
    }
    return std::move(root);
}
}  // namespace Utils
