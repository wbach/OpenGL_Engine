#pragma once
#include <string>

namespace rapidxml
{
    template<class Ch> class xml_node;
}

namespace XMLParser
{
    void ParseXML(rapidxml::xml_node<char>* node, std::string& str);
    void ParseXML(rapidxml::xml_node<char>* node, int& value);
    void ParseXML(rapidxml::xml_node<char>* node, float& value);
    void ParseXML(rapidxml::xml_node<char>* node, double& value);
    void ParseXML(rapidxml::xml_node<char>* node, bool& value);
    void ParseXML(rapidxml::xml_node<char>* node, unsigned int& value);
    void ParseXML(rapidxml::xml_node<char>* node, unsigned long& value);
    void ParseXML(rapidxml::xml_node<char>* node, unsigned long long& value);
}
