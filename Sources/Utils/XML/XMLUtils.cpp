#include "XMLUtils.h"
#include <string>

#include "Logger/Log.h"
#include "Utils.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

#include "XmlReader.h"

namespace Utils
{
RapidNodeData GetRapidNodeData(rapidxml::xml_node<char>* node)
{
    return {node->name(), node->value()};
}

RapidNodeData GetRapidAttributeData(rapidxml::xml_node<char>* node, const std::string& atttributeName)
{
    auto att = node->first_attribute(atttributeName.c_str());
    if (att == 0)
        return RapidNodeData();

    return GetRapidAttributeData(att);
}

RapidNodeData GetRapidAttributeData(rapidxml::xml_attribute<char>* node)
{
    return {node->name(), node->value()};
}

void ForEachSubNode(rapidxml::xml_node<char>* parent,
                    std::function<void(const RapidNodeData&, rapidxml::xml_node<char>*)> func)
{
    for (auto snode = parent->first_node(); snode; snode = snode->next_sibling())
    {
        auto node_data = Utils::GetRapidNodeData(snode);
        func(node_data, snode);
    }
}

void ForEachAttribute(rapidxml::xml_attribute<char>* att,
                      std::function<void(const RapidNodeData&, rapidxml::xml_attribute<char>*)> func)
{
}

void AddVectorToNode(rapidxml::xml_document<char>& document, rapidxml::xml_node<char>* node, const glm::vec3 vector)
{
    rapidxml::xml_node<char>* x =
        document.allocate_node(rapidxml::node_element, "x", document.allocate_string(std::to_string(vector.x).c_str()));
    rapidxml::xml_node<char>* y = document.allocate_node(rapidxml::node_element, "y",
                                                         document.allocate_string((std::to_string(vector.y).c_str())));
    rapidxml::xml_node<char>* z = document.allocate_node(rapidxml::node_element, "z",
                                                         document.allocate_string((std::to_string(vector.z).c_str())));
    node->append_node(x);
    node->append_node(y);
    node->append_node(z);
}

void AddVectorToNode(rapidxml::xml_document<char>& document, rapidxml::xml_node<char>* node, const glm::vec2 vector)
{
    rapidxml::xml_node<char>* x =
        document.allocate_node(rapidxml::node_element, "x", document.allocate_string(std::to_string(vector.x).c_str()));
    rapidxml::xml_node<char>* y = document.allocate_node(rapidxml::node_element, "y",
                                                         document.allocate_string((std::to_string(vector.y).c_str())));
    node->append_node(x);
    node->append_node(y);
}

std::string MessageBuilder(std::multimap<std::string, std::string>& messeges)
{
    rapidxml::xml_document<char> document;

    rapidxml::xml_node<char>* node = document.allocate_node(rapidxml::node_element, "msg");

    for (const auto& pair : messeges)
    {
        rapidxml::xml_node<char>* subnode =
            document.allocate_node(rapidxml::node_element, document.allocate_string(pair.first.c_str()),
                                   document.allocate_string(pair.second.c_str()));
        node->append_node(subnode);
    }

    document.append_node(node);

    std::stringstream ss;
    // ss << document;
    document.clear();
    return ss.str();
}

bool CheckXmlObjectType(const std::string& file, const std::string& type)
{
    XmlReader reader;
    if (reader.Read(file))
    {
        auto root = reader.Get();
        return (root and root->name() == type);
    }
    return false;
}
}  // namespace Utils
