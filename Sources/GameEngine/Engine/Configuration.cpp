#include "Configuration.h"
#include "EngineDef.h"
#include "Logger/Log.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "Utils/XML/XmlReader.h"
#include <fstream>

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

namespace GameEngine
{
	std::vector<std::string> requiredFiles;

	std::string GetFullDataPath(const std::string& file_name, bool addToRequierd)
	{
		auto path = EngineConf.files.data + file_name;
		if (addToRequierd)
			requiredFiles.push_back(path);
		return path;
	}

	std::string GetFullShaderPath(const std::string& file_name, bool addToRequierd)
	{
		auto path = EngineConf.files.shaders + file_name;
		if (addToRequierd)
			requiredFiles.push_back(path);
		return path;
	}

	std::string GetFilePatch(const std::string& file_full_path)
	{
		if (file_full_path.empty())
			return {};

		auto data_index = file_full_path.find(EngineConf.files.data);
		uint32 size = EngineConf.files.data.size();

		if (data_index == std::string::npos)
		{
			data_index = file_full_path.find(EngineConf.files.shaders);
			size = EngineConf.files.shaders.size();
		}

		auto str = file_full_path.substr(data_index + size);

		return str;
	}

	void AddRequiredFile(const std::string& file)
	{
		requiredFiles.push_back(file);
	}

	void SaveRequiredFiles()
	{
		std::ofstream output(GetFullDataPath(EngineConf.files.requiredFilesOutputFile));

		if (!output.is_open())
		{
			Log("Cant open file : " + GetFullDataPath(EngineConf.files.requiredFilesOutputFile));
			return;
		}

		for (const auto& str : requiredFiles)
		{
			output << str << std::endl;
		}
		output.close();
	}


	void Read(Utils::XmlNode& node, Params::Window& window)
	{
		window.name			= node.attributes_[CSTR_WINDOW];
		window.size.x		= Utils::StringToInt(node.attributes_[CSTR_WINDOW_WIDTH]);
		window.size.y		= Utils::StringToInt(node.attributes_[CSTR_WINDOW_HEIGHT]);
		window.fullScreen	= Utils::StringToBool(node.attributes_[CSTR_WINDOW_FULLSCREEN]);
	}

	void Read(Utils::XmlNode& node, Params::Sound& sound)
	{
		sound.isEnabled = Utils::StringToBool(node.attributes_[CSTR_SOUND_ENABLED]);
		sound.volume	= Utils::StringToFloat(node.attributes_[CSTR_SOUND_VOLUME]);
	}

	void Read(Utils::XmlNode& node, Params::Shadows& shadows)
	{
		shadows.isEnabled	= Utils::StringToBool(node.attributes_[CSTR_SHADOWS_ENABLED]);
		shadows.distance	= Utils::StringToFloat(node.attributes_[CSTR_SHADOWS_VIEW_DISTANCE]);
		shadows.mapSize		= Utils::StringToInt(node.attributes_[CSTR_SHADOWS_MAP_SIZE]);
	}

	void Read(Utils::XmlNode& node, Params::Particles& particles)
	{
		particles.useParticles = Utils::StringToBool(node.attributes_[CSTR_PARTICLES_ENABLED]);
	}

	void Read(Utils::XmlNode& node, Params::Flora& flora)
	{
		flora.isEnabled		= Utils::StringToBool(node.attributes_[CSTR_FLORA_ENABLED]);
		flora.isGrass		= Utils::StringToBool(node.attributes_[CSTR_FLORA_GRASS]);
		flora.viewDistance	= Utils::StringToFloat(node.attributes_[CSTR_FLORA_VIEW_DISTANCE]);
	}

	void Read(Utils::XmlNode& node, Params::Water& water)
	{
		water.type = static_cast<Params::WaterType>(Utils::StringToInt(node.attributes_[CSTR_WATER_TYPE]));

		water.waterReflectionResolution.x = Utils::StringToInt(node.children_[CSTR_WATER_REFLECTION].attributes_[CSTR_WATER_REFLECTION_WIDTH]);
		water.waterReflectionResolution.y = Utils::StringToInt(node.children_[CSTR_WATER_REFLECTION].attributes_[CSTR_WATER_REFLECTION_HEIGHT]);

		water.waterRefractionResolution.x = Utils::StringToInt(node.children_[CSTR_WATER_REFRACTION].attributes_[CSTR_WATER_REFLECTION_WIDTH]);
		water.waterRefractionResolution.y = Utils::StringToInt(node.children_[CSTR_WATER_REFRACTION].attributes_[CSTR_WATER_REFLECTION_HEIGHT]);
	}

	void Read(Utils::XmlNode& node, Params::Textures& textures)
	{
		textures.maxSize.x		= Utils::StringToInt(node.attributes_[CSTR_TEXTURE_MAX_RESOLUTION_WIDTH]);
		textures.maxSize.y		= Utils::StringToInt(node.attributes_[CSTR_TEXTURE_MAX_RESOLUTION_HEIGHT]);
		textures.useAmbient		= Utils::StringToBool(node.attributes_[CSTR_TEXTURE_AMBIENT]);
		textures.useDiffuse		= Utils::StringToBool(node.attributes_[CSTR_TEXTURE_DIFFUSE]);
		textures.useNormal		= Utils::StringToBool(node.attributes_[CSTR_TEXTURE_NORMAL]);
		textures.useSpecular	= Utils::StringToBool(node.attributes_[CSTR_TEXTURE_SPECULAR]);
	}

	void Read(Utils::XmlNode& node, Params::Renderer& renderer)
	{
		renderer.type = static_cast<Params::RendererType>(Utils::StringToInt(node.attributes_[CSTR_RENDERER_TYPE]));
		renderer.viewDistance = Utils::StringToFloat(node.attributes_[CSTR_RENDERER_VIEW_DISTANCE]);
		renderer.fpsLimt = Utils::StringToInt(node.attributes_[CSTR_RENDERER_FPS_LIMIT]);
		renderer.resolution.x = Utils::StringToInt(node.attributes_[CSTR_RENDERER_FPS_RESOLUTION_X]);
		renderer.resolution.y = Utils::StringToInt(node.attributes_[CSTR_RENDERER_FPS_RESOLUTION_Y]);

		Read(node.children_[CSTR_WATER], renderer.water);
		Read(node.children_[CSTR_FLORA], renderer.flora);
		Read(node.children_[CSTR_SHADOWS], renderer.shadows);
		Read(node.children_[CSTR_TEXTURES], renderer.textures);
		Read(node.children_[CSTR_PARTICLES], renderer.particles);
	}

	void Read(Utils::XmlNode& node, Params::Files& files)
	{
		files.data		= GetDataLocationFromString(node.children_[CSTR_DATA_LOCATION].value_);
		files.shaders	= GetShaderLocationFromString(node.children_[CSTR_SHADER_LOCATION].value_);
		files.requiredFilesOutputFile = node.children_[CSTR_REQUIRED_FILE_OUTPUT].value_;
	}

	void ReadFromFile(const std::string& filename)
	{
		Utils::XmlReader xmlReader;
		if (!xmlReader.Read(filename))
			return;

		Read(*xmlReader.Get(CSTR_WINDOW),	EngineConf.window);
		Read(*xmlReader.Get(CSTR_SOUND),	EngineConf.sound);
		Read(*xmlReader.Get(CSTR_FILES),	EngineConf.files);
		Read(*xmlReader.Get(CSTR_RENDERER), EngineConf.renderer);
		EngineConf.useBinaryLoading = Utils::StringToBool(xmlReader.Get(CSTR_ENABLE_BINARY_LOADING)->value_);
		AddRequiredFile(filename);
	}
} // GameEngine
