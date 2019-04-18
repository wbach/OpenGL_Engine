#include "XmlReader.h"
#include <algorithm>
#include "Logger/Log.h"
#include "Utils/Utils.h"
#include "Utils/XML/XMLUtils.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

namespace Utils
{
void ParseNode(rapidxml::xml_node<>* node, XmlNode& n)
{
    if (node == nullptr)
        return;

    n.value_ = node->value();

    for (auto att_node = node->first_attribute(); att_node; att_node = att_node->next_attribute())
    {
        std::string att_name  = att_node->name();
        std::string att_value = att_node->value();

        n.attributes_[att_name] = att_value;
    }

    for (auto snode = node->first_node(); snode; snode = snode->next_sibling())
    {
        if (snode == nullptr)
            return;

        std::string name = snode->name();
        if (name.empty())
            return;
        auto& child = n.AddChild(name);
        ParseNode(snode, child);
        child.parent = &n;
    }
}

bool XmlReader::Read(const std::string& filename)
{
    auto str = Utils::ReadFile(filename);

    if (str.empty())
    {
        return false;
    }

    return ReadXml(str);
}

bool XmlReader::ReadXml(std::string& fileContent)
{
    rapidxml::xml_document<> document;

    try
    {
        document.parse<0>(const_cast<char*>(fileContent.c_str()));
    }
    catch (const rapidxml::parse_error& p)
    {
        std::string out = p.what();
        ERROR_LOG(out);
        ERROR_LOG(fileContent);
        return false;
    }
    root_ = std::make_unique<XmlNode>(document.first_node()->name());
    ParseNode(document.first_node(), *root_);
    return true;
}

XmlNode* XmlReader::Get(const std::string& name, XmlNode* node)
{
    if (node == nullptr)
        node = root_.get();

    if (node->GetName() == name)
        return node;

    auto childIter = std::find_if(node->GetChildren().begin(), node->GetChildren().end(),
                                  [&name](const auto& child) { return (child->GetName() == name); });

    if (childIter != node->GetChildren().end())
    {
        return childIter->get();
    }

    for (const auto& child : node->GetChildren())
    {
        auto n = Get(name, child.get());
        if (n)
        {
            return n;
        }
    }

    return nullptr;
}
}  // namespace Utils
