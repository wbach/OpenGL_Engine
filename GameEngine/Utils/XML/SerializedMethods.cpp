#include "SerializedMethods.hpp"
#include <rapidxml.hpp>

namespace XMLParser
{
    void Serialize(rapidxml::xml_document<char>& document, const std::string& tag, const std::string& str, rapidxml::xml_node<char>* node)
    {
        node->append_node(document.allocate_node(rapidxml::node_element, document.allocate_string(tag.c_str()), document.allocate_string(str.c_str())));
    }
    void Serialize(rapidxml::xml_document<char>& document, const std::string& tag, int value, rapidxml::xml_node<char>* node)
    {
        Serialize(document, tag, std::to_string(value), node);
    }
    void Serialize(rapidxml::xml_document<char>& document, const std::string& tag, float value, rapidxml::xml_node<char>* node)
    {
        Serialize(document, tag, std::to_string(value), node);
    }
    void Serialize(rapidxml::xml_document<char>& document, const std::string& tag, double value, rapidxml::xml_node<char>* node)
    {
        Serialize(document, tag, std::to_string(value), node);
    }
    void Serialize(rapidxml::xml_document<char>& document, const std::string& tag, bool value, rapidxml::xml_node<char>* node)
    {
        Serialize(document, tag, value ? std::string("TRUE") : std::string("FALSE"), node);
    }
    void Serialize(rapidxml::xml_document<char>& document, const std::string& tag, unsigned int value, rapidxml::xml_node<char>* node)
    {
        Serialize(document, tag, std::to_string(value), node);
    }
    void Serialize(rapidxml::xml_document<char>& document, const std::string& tag, unsigned long value, rapidxml::xml_node<char>* node)
    {
        Serialize(document, tag, std::to_string(value), node);
    }
    void Serialize(rapidxml::xml_document<char>& document, const std::string& tag, unsigned long long value, rapidxml::xml_node<char>* node)
    {
        Serialize(document, tag, std::to_string(value), node);
    }
}
