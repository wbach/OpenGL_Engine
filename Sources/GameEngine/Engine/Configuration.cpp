#include "Configuration.h"
#include "EngineDef.h"
#include "Logger/Log.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include <fstream>

namespace XMLParser
{
wb::vec2i ParseWithHeight(rapidxml::xml_node<>* node)
{
    wb::vec2i v;

    for (auto att_node = node->first_attribute(); att_node; att_node = att_node->next_attribute())
    {
        std::string att_name  = att_node->name();
        std::string att_value = att_node->value();

        if (att_name == "width")
            v.x = Utils::StringToInt(att_value);
        if (att_name == "height")
            v.y = Utils::StringToInt(att_value);
    }
    return v;
}

void ParseWindow(rapidxml::xml_node<>* node, EngineConfiguration& config)
{
    for (auto att_node = node->first_attribute(); att_node; att_node = att_node->next_attribute())
    {
        std::string name  = att_node->name();
        std::string value = att_node->value();

        if (name == "name")
            config.windowName = value;
        else if (name == "width")
            config.resolution.x = Utils::StringToInt(value);
        else if (name == "height")
            config.resolution.y = Utils::StringToInt(value);
        else if (name == "fullscreen")
            config.fullScreen = Utils::StringToBool(value);
        else if (name == "refreshRate")
            config.refresRate = Utils::StringToFloat(value);
		else if (name == "vsync")
			config.vsync = Utils::StringToBool(value);
    }
}

void ParseSound(rapidxml::xml_node<>* node, EngineConfiguration& config)
{
    for (auto att_node = node->first_attribute(); att_node; att_node = att_node->next_attribute())
    {
        std::string name  = att_node->name();
        std::string value = att_node->value();

        if (name == "enabled")
            config.sound = Utils::StringToBool(value);
        else if (name == "volume")
            config.soundVolume = Utils::StringToFloat(value);
    }
}

void ParseTextureMaxResolution(rapidxml::xml_node<>* node, EngineConfiguration& config)
{
    config.maxTextureResolutuion = ParseWithHeight(node);
}

void ParseFloraConfig(rapidxml::xml_node<>* node, EngineConfiguration& config)
{
    for (auto att_node = node->first_attribute(); att_node; att_node = att_node->next_attribute())
    {
        std::string att_name  = att_node->name();
        std::string att_value = att_node->value();

        if (att_name == "viewDistance")
            config.floraViewDistance = Utils::StringToFloat(att_value);
        if (att_name == "grass")
            config.advancedGrass = Utils::StringToBool(att_value);
    }
}

void ParseParticlesConfig(rapidxml::xml_node<>* node, EngineConfiguration& config)
{
	for (auto att_node = node->first_attribute(); att_node; att_node = att_node->next_attribute())
	{
		std::string att_name = att_node->name();
		std::string att_value = att_node->value();

		if (att_name == "enabled")
			config.useParticles = Utils::StringToBool(att_value);
	}
}

void ParseShadowsConfig(rapidxml::xml_node<>* node, EngineConfiguration& config)
{
    for (auto att_node = node->first_attribute(); att_node; att_node = att_node->next_attribute())
    {
        std::string att_name  = att_node->name();
        std::string att_value = att_node->value();

        if (att_name == "enabled")
            config.isShadows = Utils::StringToBool(att_value);
        if (att_name == "distance")
            config.shadowsDistance = Utils::StringToFloat(att_value);
        if (att_name == "shadowMapSize")
            config.shadowMapSize = Utils::StringToInt(att_value);
    }
}

void ParseWaterConfig(rapidxml::xml_node<>* node, EngineConfiguration& config)
{
    for (auto snode = node->first_node(); snode; snode = snode->next_sibling())
    {
        std::string name  = snode->name();
        std::string value = snode->value();

        if (name == "Reflection")
            config.waterReflectionResolution = ParseWithHeight(snode);
        if (name == "Refraction")
            config.waterRefractionResolution = ParseWithHeight(snode);
    }

    for (auto att_node = node->first_attribute(); att_node; att_node = att_node->next_attribute())
    {
        std::string att_name  = att_node->name();
        std::string att_value = att_node->value();

        if (att_name == "quality")
            config.isHighWaterQuality = Utils::StringToBool(att_value);
    }
}

void ParseRenderer(rapidxml::xml_node<>* node, EngineConfiguration& config)
{
    for (auto snode = node->first_node(); snode; snode = snode->next_sibling())
    {
        std::string name  = snode->name();
        std::string value = snode->value();

        if (name == "type")
            config.rendererType = static_cast<EngineConfiguration::RendererType>(Utils::StringToInt(value));

        if (name == "start_lod_lvl")
            config.startLod = static_cast<LoD>(Utils::StringToInt(value));
    }
}

std::string GetDataLocationFromString(const std::string& str)
{
	if (str.empty())
		return DEFAULT_DATA_PATH;

	if (str[str.size() - 1] != '/')
	{
		return str + '/';
	}

	return str;
}

std::string GetShaderLocationFromString(const std::string& str)
{
	if (str.empty())
		return DEFAULT_SHADERS_PATH;

	if (str[str.size() - 1] != '/')
	{
		return str + '/';
	}

	return str;
}

void ParseEngineConfig(rapidxml::xml_node<>* node, EngineConfiguration& config)
{
    for (auto snode = node->first_node(); snode; snode = snode->next_sibling())
    {
        std::string name  = snode->name();
        std::string value = snode->value();

        if (name == "ViewDistance")
            config.viewDistance = Utils::StringToFloat(value);
        if (name == "Renderer")
            ParseRenderer(snode, config);
        if (name == "TextureMaxResolution")
            ParseTextureMaxResolution(snode, config);
        if (name == "Flora")
            ParseFloraConfig(snode, config);
		if (name == "Particles")
			ParseParticlesConfig(snode, config);
        if (name == "Shadows")
            ParseShadowsConfig(snode, config);
        if (name == "Water")
            ParseWaterConfig(snode, config);
        if (name == "DataLocation")
            config.dataFilesLocation = GetDataLocationFromString(value);
        if (name == "ShadersLocation")
            config.shadersFilesLocation = GetShaderLocationFromString(value);
		if (name == "EnableBinaryLoading")
			config.enableBinaryLoading = Utils::StringToBool(value);
    }
}

void CreateConfigFile(const std::string& filename)
{

}

EngineConfiguration& ReadConfigFile(const std::string& file)
{
    auto& config = EngineConf;

    auto str = Utils::ReadFile(file);

	if (str.empty())
	{
		CreateConfigFile(file);
		return config;
	}

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
    for (auto snode = root->first_node(); snode; snode = snode->next_sibling())
    {
        std::string name = snode->name();
        if (name == "Window")
            ParseWindow(snode, config);
        else if (name == "Sound")
            ParseSound(snode, config);
        else if (name == "Engine")
            ParseEngineConfig(snode, config);
    }

    document.clear();

    return config;
}
}

std::string EngineConfiguration::GetFullDataPath(const std::string& file_name, bool addToRequierd)
{
    auto path = dataFilesLocation + file_name;
    if (addToRequierd)
        requiredFiles.push_back(path);
    return path;
}

std::string EngineConfiguration::GetFullShaderPath(const std::string& file_name, bool addToRequierd)
{
    auto path = shadersFilesLocation + file_name;
    if (addToRequierd)
        requiredFiles.push_back(path);
    return path;
}

std::string EngineConfiguration::GetFilePatch(const std::string& file_full_path) const
{
    if (file_full_path.empty())
        return {};

    auto data_index = file_full_path.find(dataFilesLocation);
    uint32 size     = dataFilesLocation.size();

    if (data_index == std::string::npos)
    {
        data_index = file_full_path.find(shadersFilesLocation);
        size       = shadersFilesLocation.size();
    }

    auto str = file_full_path.substr(data_index + size);

    return str;
}

void EngineConfiguration::AddRequiredFile(const std::string& file)
{
    requiredFiles.push_back(file);
}

void EngineConfiguration::SaveRequiredFiles()
{
    std::ofstream output(GetFullDataPath(requiredFilesOutputFile));

    if (!output.is_open())
    {
        Log("Cant open file : " + GetFullDataPath(requiredFilesOutputFile));		
        return;
    }

    for (const auto& str : requiredFiles)
    {
        /*while (1)
        {
                auto i = str.find("../");
                if (i == std::string::npos)
                        break;

                str = str.substr(i+3);
        }*/

        output << str << std::endl;
    }
    output.close();
}

#include "Utils/XML/XmlReader.h"


void EngineConfiguration::ReadFromFile(const std::string& filename)
{
   // XMLParser::ReadConfigFile(filename);
	Utils::XmlReader xmlReader;
	xmlReader.Read(filename);

	auto window = xmlReader.Get(NODE_WINDOW);

	windowName = window->attributes_[PAR_WINDOW_NAME];
	vsync = Utils::StringToBool(window->attributes_[PAR_WINDOW_VSYNC]);
	refresRate = Utils::StringToFloat(window->attributes_[PAR_WINDOW_REFRESH_RATE]);
	windowSize.x = Utils::StringToInt(window->attributes_[PAR_WINDOW_WIDTH]);
	windowSize.x = Utils::StringToInt(window->attributes_[PAR_WINDOW_HEIGHT]);

	auto sound_node = xmlReader.Get(NODE_SOUND);

	sound = Utils::StringToBool(sound_node->attributes_[PAR_SOUND_ENABLED]);
	soundVolume = Utils::StringToFloat(sound_node->attributes_[PAR_SOUND_VOLUME]);

	auto engine_node = xmlReader.Get(NODE_ENGINE);

	auto shadows_node = engine_node->children_[NODE_SHADOWS];

	isShadows = Utils::StringToBool(shadows_node->attributes_[PAR_SHADOWS_ENABLED]);
	shadowsDistance = Utils::StringToBool(shadows_node->attributes_[PAR_SHADOWS_DISTANCE]);
	shadowMapSize = Utils::StringToInt(shadows_node->attributes_[PAR_SHADOWS_MAP_SIZE]);

	startLod = static_cast<LoD>(Utils::StringToInt(engine_node->children_[NODE_RENDERER_START_LOD_LVL]->value_));
	rendererType = static_cast<RendererType>(Utils::StringToInt(engine_node->children_[NODE_RENDERER_TYPE]->value_));

	dataFilesLocation = engine_node->children_[NODE_DATA_LOCATION]->value_;
	shadersFilesLocation = engine_node->children_[NODE_SHADER_LOCATION]->value_;

	auto& particles_node = engine_node->children_[NODE_PARTICLES];
	useParticles = Utils::StringToBool(particles_node->value_);

	auto& textures_max_resolution_node = engine_node->children_[NODE_TEXTURE_MAX_RESOLUTION];

	maxTextureResolutuion.x = Utils::StringToInt(textures_max_resolution_node->children_[NODE_RENDERER_START_LOD_LVL]->value_);
	maxTextureResolutuion.y = Utils::StringToInt(textures_max_resolution_node->children_[NODE_RENDERER_TYPE]->value_);

	//auto& textures_max_resolution_node = engine_node->children_[NODE_TEXTURE_MAX_RESOLUTION];

	AddRequiredFile(filename);
}
