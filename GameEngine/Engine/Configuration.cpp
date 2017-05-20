#include "Configuration.h"
#include "../Debug_/Log.h"
#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"

namespace XMLParser
{
	wb::vec2i ParseWithHeight(rapidxml::xml_node<>* node)
	{
		wb::vec2i v;

		for (auto att_node = node->first_attribute(); att_node; att_node = att_node->next_attribute())
		{
			std::string att_name = att_node->name();
			std::string att_value = att_node->value();

			if (att_name == "width")
				v.x = Utils::StringToInt(att_value);
			if (att_name == "height")
				v.y = Utils::StringToInt(att_value);
		}
		return v;
	}

	void ParseWindow(rapidxml::xml_node<>* node, SConfiguration& config)
	{
		for (auto att_node = node->first_attribute(); att_node; att_node = att_node->next_attribute())
		{
			std::string name = att_node->name();
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
		}
	}

	void ParseSound(rapidxml::xml_node<>* node, SConfiguration& config)
	{
		for (auto att_node = node->first_attribute(); att_node; att_node = att_node->next_attribute())
		{
			std::string name = att_node->name();
			std::string value = att_node->value();

			if (name == "enabled")
				config.sound = Utils::StringToBool(value);
			else if (name == "volume")
				config.soundVolume = Utils::StringToFloat(value);
		}
	}	

	void ParseTextureMaxResolution(rapidxml::xml_node<>* node, SConfiguration& config)
	{
		config.maxTextureResolutuion = ParseWithHeight(node);
	}

	void ParseFloraConfig(rapidxml::xml_node<>* node, SConfiguration& config)
	{
		for (auto att_node = node->first_attribute(); att_node; att_node = att_node->next_attribute())
		{
			std::string att_name = att_node->name();
			std::string att_value = att_node->value();

			if (att_name == "viewDistance")
				config.floraViewDistance = Utils::StringToFloat(att_value);
			if (att_name == "grass")
				config.advancedGrass = Utils::StringToBool(att_value);
		}
	}

	void ParseShadowsConfig(rapidxml::xml_node<>* node, SConfiguration& config)
	{
		for (auto att_node = node->first_attribute(); att_node; att_node = att_node->next_attribute())
		{
			std::string att_name = att_node->name();
			std::string att_value = att_node->value();

			if (att_name == "enabled")
				config.isShadows = Utils::StringToBool(att_value);
			if (att_name == "distance")
				config.shadowsDistance = Utils::StringToFloat(att_value);
			if (att_name == "shadowMapSize")
				config.shadowMapSize = Utils::StringToInt(att_value);
		}
	}

	void ParseWaterConfig(rapidxml::xml_node<>* node, SConfiguration& config)
	{
		for (auto snode = node->first_node(); snode; snode = snode->next_sibling())
		{
			std::string name = snode->name();
			std::string value = snode->value();

			if (name == "Reflection")
				config.waterReflectionResolution = ParseWithHeight(snode);
			if (name == "Refraction")
				config.waterRefractionResolution = ParseWithHeight(snode);
		}

		for (auto att_node = node->first_attribute(); att_node; att_node = att_node->next_attribute())
		{
			std::string att_name = att_node->name();
			std::string att_value = att_node->value();

			if (att_name == "quality")
				config.isHighWaterQuality = Utils::StringToBool(att_value);
		}
	}

	void ParseRenderer(rapidxml::xml_node<>* node, SConfiguration& config)
	{
		for (auto snode = node->first_node(); snode; snode = snode->next_sibling())
		{
			std::string name = snode->name();
			std::string value = snode->value();

			if(name == "type")
				config.rendererType = static_cast<SConfiguration::RendererType>(Utils::StringToInt(value));

			if (name == "start_lod_lvl")
				config.startLod = static_cast<LoD>(Utils::StringToInt(value));

		}
	}

	void ParseEngineConfig(rapidxml::xml_node<>* node, SConfiguration& config)
	{
		for (auto snode = node->first_node(); snode; snode = snode->next_sibling())
		{
			std::string name = snode->name();
			std::string value = snode->value();

			if (name == "ViewDistance")
				config.viewDistance = Utils::StringToFloat(value);
			if (name == "Renderer")
				ParseRenderer(snode, config);
			if (name == "TextureMaxResolution")
				ParseTextureMaxResolution(snode, config);
			if (name == "Flora")
				ParseFloraConfig(snode, config);
			if (name == "Shadows")
				ParseShadowsConfig(snode, config);
			if (name == "Water")
				ParseWaterConfig(snode, config);
		}
	}

	SConfiguration& ReadConfigFile(const std::string& file)
	{
		auto& config = SConfiguration::Instance();

		auto str = Utils::ReadFile(file);

		if (str.empty())
			return config;

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

SConfiguration & SConfiguration::Instance()
{
	static SConfiguration conf;
	return conf;
}

void SConfiguration::ReadFromFile(const std::string & filename)
{
	XMLParser::ReadConfigFile(filename);
}
