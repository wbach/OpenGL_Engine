#include "ConfigurationWriter.h"
#include "Configuration.h"
#include "Utils/XML/XmlWriter.h"
#include "EngineDef.h"

namespace GameEngine
{
	using namespace Utils;

	void Create(std::shared_ptr<XmlNode>& node, const Params::Textures& param)
	{
		node = std::make_shared<XmlNode>();
		node->name_ = CSTR_TEXTURES;
		node->attributes_[CSTR_TEXTURE_MAX_RESOLUTION_WIDTH]  = std::to_string(param.maxSize.x);
		node->attributes_[CSTR_TEXTURE_MAX_RESOLUTION_HEIGHT] = std::to_string(param.maxSize.y);
		node->attributes_[CSTR_TEXTURE_AMBIENT]  = Utils::BoolToString(param.useAmbient);
		node->attributes_[CSTR_TEXTURE_DIFFUSE]  = Utils::BoolToString(param.useDiffuse);
		node->attributes_[CSTR_TEXTURE_NORMAL]   = Utils::BoolToString(param.useNormal);
		node->attributes_[CSTR_TEXTURE_SPECULAR] = Utils::BoolToString(param.useSpecular);
	}
	void Create(std::shared_ptr<XmlNode>& node, const Params::Particles& param)
	{
		node = std::make_shared<XmlNode>();
		node->name_ = CSTR_PARTICLES;
		node->attributes_[CSTR_PARTICLES_ENABLED] = Utils::BoolToString(param.useParticles);
	}
	void Create(std::shared_ptr<XmlNode>& node, const Params::Shadows& param)
	{
		node = std::make_shared<XmlNode>();
		node->name_ = CSTR_SHADOWS;
		node->attributes_[CSTR_SHADOWS_ENABLED]		  = Utils::BoolToString(param.isEnabled);
		node->attributes_[CSTR_SHADOWS_VIEW_DISTANCE] = std::to_string(param.distance);
		node->attributes_[CSTR_SHADOWS_MAP_SIZE]	  = std::to_string(param.mapSize);
	}
	void Create(std::shared_ptr<XmlNode>& node, const Params::Flora& param)
	{
		node = std::make_shared<XmlNode>();
		node->name_ = CSTR_FLORA;
		node->attributes_[CSTR_FLORA_ENABLED]		= Utils::BoolToString(param.isEnabled);
		node->attributes_[CSTR_FLORA_GRASS]			= std::to_string(param.isGrass);
		node->attributes_[CSTR_FLORA_VIEW_DISTANCE] = std::to_string(param.viewDistance);
	}
	void Create(std::shared_ptr<XmlNode>& node, const Params::Water& param)
	{
		node = std::make_shared<XmlNode>();
		node->name_ = CSTR_WATER;
		node->attributes_[CSTR_WATER_TYPE] = std::to_string(static_cast<uint32>(param.type));

		auto& reflection = node->children_[CSTR_WATER_REFLECTION] = std::make_shared<XmlNode>();
		reflection->name_ = CSTR_WATER_REFLECTION;
		reflection->attributes_[CSTR_WATER_REFLECTION_WIDTH] = std::to_string(param.waterReflectionResolution.x);
		reflection->attributes_[CSTR_WATER_REFLECTION_HEIGHT] = std::to_string(param.waterReflectionResolution.y);

		auto& refraction = node->children_[CSTR_WATER_REFRACTION] = std::make_shared<XmlNode>();
		refraction->name_ = CSTR_WATER_REFRACTION;
		refraction->attributes_[CSTR_WATER_REFRACTION_WIDTH] = std::to_string(param.waterRefractionResolution.x);
		refraction->attributes_[CSTR_WATER_REFRACTION_HEIGHT] = std::to_string(param.waterRefractionResolution.y);
	}
	void Create(std::shared_ptr<XmlNode>& node, const Params::Renderer& param)
	{
		node = std::make_shared<XmlNode>();
		node->name_ = CSTR_RENDERER;
		node->attributes_[CSTR_RENDERER_TYPE] = std::to_string(static_cast<uint32>(param.type));
		node->attributes_[CSTR_RENDERER_FPS_RESOLUTION_X] = std::to_string(param.resolution.x);
		node->attributes_[CSTR_RENDERER_FPS_RESOLUTION_Y] = std::to_string(param.resolution.y);
		node->attributes_[CSTR_RENDERER_FPS_LIMIT] = std::to_string(param.fpsLimt);
		Create(node->children_[CSTR_WATER], param.water);
		Create(node->children_[CSTR_FLORA], param.flora);
		Create(node->children_[CSTR_SHADOWS], param.shadows);
		Create(node->children_[CSTR_TEXTURES], param.textures);
		Create(node->children_[CSTR_PARTICLES], param.particles);
	}
	void Create(std::shared_ptr<XmlNode>& node, const Params::Window& param)
	{
		node = std::make_shared<XmlNode>();
		node->name_ = CSTR_WINDOW;
		node->attributes_[CSTR_WINDOW_NAME] = param.name;
		node->attributes_[CSTR_WINDOW_WIDTH] = std::to_string(param.size.x);
		node->attributes_[CSTR_WINDOW_HEIGHT] = std::to_string(param.size.y);
		node->attributes_[CSTR_WINDOW_FULLSCREEN] = Utils::BoolToString(param.fullScreen);
	}
	void Create(std::shared_ptr<XmlNode>& node, const Params::Files& param)
	{
		node = std::make_shared<XmlNode>();
		node->name_ = CSTR_FILES;
		auto& dataFiles = node->children_[CSTR_DATA_LOCATION] = std::make_shared<XmlNode>();
		dataFiles->name_ = CSTR_DATA_LOCATION;
		dataFiles->value_ = param.data;
		auto& shaderFiles = node->children_[CSTR_SHADER_LOCATION] = std::make_shared<XmlNode>();
		shaderFiles->value_ = param.shaders;
		shaderFiles->name_ = CSTR_SHADER_LOCATION;
		auto& reqFiles = node->children_[CSTR_REQUIRED_FILE_OUTPUT] = std::make_shared<XmlNode>();
		reqFiles->name_ = CSTR_REQUIRED_FILE_OUTPUT;
		reqFiles->value_ = param.requiredFilesOutputFile;
	}
	void Create(std::shared_ptr<XmlNode>& node, const Params::Sound& param)
	{
		node = std::make_shared<XmlNode>();
		node->name_ = CSTR_SOUND;
		node->attributes_[CSTR_SOUND_ENABLED] = std::to_string(param.isEnabled);
		node->attributes_[CSTR_SOUND_VOLUME] = std::to_string(param.volume);
	}
	void CreateBinary(std::shared_ptr<XmlNode>& node, bool param)
	{
		node = std::make_shared<XmlNode>();
		node->name_ = CSTR_ENABLE_BINARY_LOADING;
		node->value_ = Utils::BoolToString(param);
	}
	void WriteConfigurationToFile(const Configuration& configuration, const std::string& filename)
	{
		XmlNode config;
		config.name_ = CSTR_CONFIG;

		Create(config.children_[CSTR_WINDOW], configuration.window);
		Create(config.children_[CSTR_FILES], configuration.files);
		Create(config.children_[CSTR_SOUND], configuration.sound);
		Create(config.children_[CSTR_RENDERER], configuration.renderer);
		CreateBinary(config.children_[CSTR_ENABLE_BINARY_LOADING], configuration.useBinaryLoading);

		Xml::Write(filename, config);
	}
} // GameEngine
