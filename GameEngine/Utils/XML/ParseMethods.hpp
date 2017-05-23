#pragma once
#include <string>

namespace rapidxml
{
    template<class Ch> class xml_node;
}

namespace XMLParser
{
    void Parse(rapidxml::xml_node<char>* node, std::string& str);
    void Parse(rapidxml::xml_node<char>* node, int& value);
    void Parse(rapidxml::xml_node<char>* node, float& value);
    void Parse(rapidxml::xml_node<char>* node, double& value);
    void Parse(rapidxml::xml_node<char>* node, bool& value);
    void Parse(rapidxml::xml_node<char>* node, unsigned int& value);
    void Parse(rapidxml::xml_node<char>* node, unsigned long& value);
    void Parse(rapidxml::xml_node<char>* node, unsigned long long& value);
}
