#pragma once
#include <string>

namespace rapidxml
{
    template<class Ch> class xml_node;
    template<class Ch> class xml_document;
}

namespace XMLParser
{
    void Serialize(rapidxml::xml_document<char>& document, const std::string& tag, const std::string& str, rapidxml::xml_node<char>* node);
    void Serialize(rapidxml::xml_document<char>& document, const std::string& tag, int value, rapidxml::xml_node<char>* node);
    void Serialize(rapidxml::xml_document<char>& document, const std::string& tag, float value, rapidxml::xml_node<char>* node);
    void Serialize(rapidxml::xml_document<char>& document, const std::string& tag, double value, rapidxml::xml_node<char>* node);
    void Serialize(rapidxml::xml_document<char>& document, const std::string& tag, bool value, rapidxml::xml_node<char>* node);
    void Serialize(rapidxml::xml_document<char>& document, const std::string& tag, unsigned int value, rapidxml::xml_node<char>* node);
    void Serialize(rapidxml::xml_document<char>& document, const std::string& tag, unsigned long value, rapidxml::xml_node<char>* node);
    void Serialize(rapidxml::xml_document<char>& document, const std::string& tag, unsigned long long value, rapidxml::xml_node<char>* node);
}
