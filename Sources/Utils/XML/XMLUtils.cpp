#include "XMLUtils.h"

#include "Logger/Log.h"
#include "Utils.h"
#include "XmlNode.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

namespace Utils
{
const std::string CSTR_X = "x";
const std::string CSTR_Y = "y";
const std::string CSTR_Z = "z";
const std::string CSTR_W = "w";

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

std::unique_ptr<XmlNode> Convert(const std::string& label, const vec2& v)
{
    // clang-format off
    auto root = std::make_unique<XmlNode>(label);
    root->attributes_ =
    {
        { CSTR_X, std::to_string(v.x) },
        { CSTR_Y, std::to_string(v.y) }
    };
    // clang-format on
    return std::move(root);
}

std::unique_ptr<XmlNode> Convert(const std::string& label, const vec3& v)
{
    // clang-format off
    auto root = std::make_unique<XmlNode>(label);
    root->attributes_ =
    {
        { CSTR_X, std::to_string(v.x) },
        { CSTR_Y, std::to_string(v.y) },
        { CSTR_Z, std::to_string(v.z) }
    };
    // clang-format on
    return std::move(root);
}

std::unique_ptr<XmlNode> Convert(const std::string& label, const Quaternion& v)
{
    auto root = std::make_unique<XmlNode>(label);
    // clang-format off
    root->attributes_ =
    {
        { CSTR_X, std::to_string(v.x) },
        { CSTR_Y, std::to_string(v.y) },
        { CSTR_Z, std::to_string(v.z) },
        { CSTR_W, std::to_string(v.w) }
    };
    // clang-format on
    return std::move(root);
}

vec2 ConvertToVec2(XmlNode& node)
{
    vec2 result;
    result.x = std::stof(node.attributes_.at("x"));
    result.y = std::stof(node.attributes_.at("y"));
    return result;
}

vec3 ConvertToVec3(XmlNode& node)
{
    vec3 result;
    result.x = std::stof(node.attributes_.at("x"));
    result.y = std::stof(node.attributes_.at("y"));
    result.z = std::stof(node.attributes_.at("z"));
    return result;
}

float ReadFloat(Utils::XmlNode& node)
{
    try
    {
        return std::stof(node.value_);
    }
    catch (...)
    {
        ERROR_LOG("read error " + node.value_);
    }
}

bool ReadBool(XmlNode& node)
{
    return Utils::StringToBool(node.value_);
}

vec3 ReadVec3(XmlNode& node)
{
    vec3 v;
    try
    {
        v.x = std::stof(node.attributes_[CSTR_X]);
        v.y = std::stof(node.attributes_[CSTR_Y]);
        v.z = std::stof(node.attributes_[CSTR_Z]);
    }
    catch (...)
    {
        ERROR_LOG("Read error");
    }
    return v;
}

vec4 ReadVec4(XmlNode& node)
{
    vec4 v(0.f);
    try
    {
        v.x = std::stof(node.attributes_[CSTR_X]);
        v.y = std::stof(node.attributes_[CSTR_Y]);
        v.z = std::stof(node.attributes_[CSTR_Z]);
        v.w = std::stof(node.attributes_[CSTR_W]);
    }
    catch (...)
    {
        ERROR_LOG("Read error");
    }
    return v;
}

Quaternion ReadQuat(XmlNode& node)
{
    Quaternion v;
    try
    {
        v.x = std::stof(node.attributes_[CSTR_X]);
        v.y = std::stof(node.attributes_[CSTR_Y]);
        v.z = std::stof(node.attributes_[CSTR_Z]);
        v.w = std::stof(node.attributes_[CSTR_W]);
    }
    catch (...)
    {
        ERROR_LOG("Read error");
    }
    return v;
}

vec2ui ReadVec2ui(XmlNode& node)
{
    vec2ui v;
    v.x = std::stoul(node.attributes_[CSTR_X]);
    v.y = std::stoul(node.attributes_[CSTR_Y]);
    return v;
}

vec2 ReadVec2(XmlNode& node)
{
    vec2 v;
    v.x = std::stof(node.attributes_[CSTR_X]);
    v.y = std::stof(node.attributes_[CSTR_Y]);
    return v;
}

std::unique_ptr<XmlNode> Convert(const std::string& v)
{
    return std::make_unique<Utils::XmlNode>("v", v);
}

}  // namespace Utils
