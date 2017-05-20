#include "BasicParseMethods.hpp"
#include <rapidxml.hpp>
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace XMLParser
{
    void ParseXML(rapidxml::xml_node<char>* node, std::string& str)
    {
        str = node->value();
    }
    void ParseXML(rapidxml::xml_node<char>* node, int& value)
    {
        std::string str(node->value());
        if (!str.empty())
        {
            try
            {
                value = std::stoi(node->value());
            }
            catch (const std::out_of_range& err)
            {
                std::cerr << "ParseXML int :  out_of_range " << err.what() << " : " << str<< std::endl;
            }
            catch (const std::invalid_argument& err)
            {
                std::cerr << "ParseXML int :  invalid_argument " << err.what() << " : " << str<< std::endl;
            }
        }
    }
    void ParseXML(rapidxml::xml_node<char>* node, float& value)
    {
        std::string str(node->value());
        if (!str.empty())
        {
            try
            {
                value = std::stof(node->value());
            }
            catch (const std::out_of_range& err)
            {
                std::cerr << "ParseXML float :  out_of_range " << err.what() << " : " << str<< std::endl;
            }
            catch (const std::invalid_argument& err)
            {
                std::cerr << "ParseXML float :  invalid_argument " << err.what() << " : " << str<< std::endl;
            }
        }
    }
    void ParseXML(rapidxml::xml_node<char>* node, double& value)
    {
        std::string str(node->value());
        if (!str.empty())
        {
            try
            {
                value = std::stod(node->value());
            }
            catch (const std::out_of_range& err)
            {
                std::cerr << "ParseXML double :  out_of_range " << err.what() << " : " << str<< std::endl;
            }
            catch (const std::invalid_argument& err)
            {
                std::cerr << "ParseXML double :  invalid_argument " << err.what() << " : " << str<< std::endl;
            }
        }
    }
    void ParseXML(rapidxml::xml_node<char>* node, bool& value)
    {
        std::string node_val(node->value());
        std::transform(node_val.begin(), node_val.end(), node_val.begin(), ::tolower);
        value = node_val == "true" ? true : false;
    }
    void ParseXML(rapidxml::xml_node<char>* node, unsigned int& value)
    {
        std::string str(node->value());
        if (!str.empty())
        {
            try
            {
                value = std::stoul(node->value());
            }
            catch (const std::out_of_range& err)
            {
                std::cerr << "ParseXML unsigned int :  out_of_range " << err.what() << " : " << str<< std::endl;
            }
            catch (const std::invalid_argument& err)
            {
                std::cerr << "ParseXML unsigned int :  invalid_argument " << err.what() << " : " << str<< std::endl;
            }
        }
    }
    void ParseXML(rapidxml::xml_node<char>* node, unsigned long& value)
    {
        std::string str(node->value());
        if (!str.empty())
        {
            try
            {
                value = std::stol(node->value());
            }
            catch (const std::out_of_range& err)
            {
                std::cerr << "ParseXML unsigned long :  out_of_range " << err.what() << " : " << str<< std::endl;
            }
            catch (const std::invalid_argument& err)
            {
                std::cerr << "ParseXML unsigned long :  invalid_argument " << err.what() << " : " << str<< std::endl;
            }
        }
    }
    void ParseXML(rapidxml::xml_node<char>* node, unsigned long long& value)
    {
        std::string str(node->value());
        if (!str.empty())
        {
            try
            {
                value = std::stoull(node->value());
            }
            catch (const std::out_of_range& err)
            {
                std::cerr << "ParseXML unsigned long long:  out_of_range " << err.what() << " : " << str<< std::endl;
            }
            catch (const std::invalid_argument& err)
            {
                std::cerr << "ParseXML unsigned long long:  invalid_argument " << err.what() << " : " << str<< std::endl;
            }
        }
    }
}
