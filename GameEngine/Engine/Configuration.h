#pragma once
#include "../Utils/Utils.h"
#include "../Debug_/Log.h"
#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"

struct SConfiguration
{
    //Window
    std::string m_WindowName = "Default Window Name";
    wb::vec2i   m_Resolution = {1000, 600};
    bool        m_FullScreen = false;
    float       m_RefresRate = 60;

    //Sound
    bool        m_Sound         = true;
    float       m_SoundVolume   = 1.f;

    //Water
    bool        m_HighWaterQuality = true;
    wb::vec2i   m_WaterReflectionResolution = {640, 480};
    wb::vec2i   m_WaterRefractionResolution = {320, 240};

    //Models
    wb::vec2i   m_TextureMaxResolution = {4096, 4096};

    //Shadows
    bool        m_Shadows = true;
    float       m_ShadowsDistance = 200.f;
    int         m_ShadowMapSize = 4096;

    //ViewDistance
    float       m_ViewDistance = 200.f;
    float       m_FloraViewDistance = 100.f;

    //Renderer
    int         m_RenderQualityl = 1;   //simple renderer/full renderer
    float       m_RenderingResolution = 1.f;
};

namespace XMLParser
{
    static void ParseWindow(rapidxml::xml_node<>* node, SConfiguration& config)
    {
        for(auto att_node = node->first_attribute(); att_node; att_node = att_node->next_attribute())
        {
            std::string name = att_node->name();
            std::string value = att_node->value();

            if(name == "name")
                config.m_WindowName = value;
            else if(name == "width")
                config.m_Resolution.x = std::stoi(value);
            else if(name == "height")
                config.m_Resolution.y = std::stoi(value);
            else if(name == "fullscreen")
                config.m_FullScreen = value == "true" ? true : false;
            else if(name == "refreshRate")
                config.m_RefresRate = std::stof(value);
        }
    }

    static void ParseSound(rapidxml::xml_node<>* node, SConfiguration& config)
    {
        for(auto att_node = node->first_attribute(); att_node; att_node = att_node->next_attribute())
        {
            std::string name = att_node->name();
            std::string value = att_node->value();

            if(name == "enabled")
                config.m_Sound = value == "true" ? true : false;
            else if(name == "volume")
                config.m_SoundVolume = std::stof(value);
        }
    }

    static void ParseEngineConfig(rapidxml::xml_node<>* node, SConfiguration& config)
    {
        for(auto snode = node->first_node(); snode; snode = snode->next_sibling())
        {
            for(auto att_node = node->first_attribute(); att_node; att_node = att_node->next_attribute())
            {
                std::string name = att_node->name();
                std::string value = att_node->value();


            }
        }
    }

    static SConfiguration ReadConfigFile(const std::string& file)
    {
        SConfiguration config;

        auto str = Utils::ReadFile(file);

        rapidxml::xml_document<> document;
        try
        {
            document.parse<0>(const_cast<char*>(str.c_str()));
        }
        catch (rapidxml::parse_error p)
        {
            p.what();
            return config;
        }

        auto root = document.first_node();
        for(auto snode = root->first_node(); snode; snode = snode->next_sibling())
        {
            std::string name = snode->name();
            if(name == "Window")
                ParseWindow(snode, config);
            else if(name == "Sound")
                ParseSound(snode, config);
            else if(name == "Engine")
                ParseEngineConfig(snode, config);
        }

        document.clear();

        return config;
    }
}