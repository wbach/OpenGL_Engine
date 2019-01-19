#include "ConfigurationWriter.h"
#include "Configuration.h"
#include "EngineDef.h"
#include "Utils/XML/XmlWriter.h"

namespace GameEngine
{
using namespace Utils;

void Create(XmlNode& node, const Params::Textures& param)
{
    node.attributes_[CSTR_TEXTURE_MAX_RESOLUTION_WIDTH]  = std::to_string(param.maxSize.x);
    node.attributes_[CSTR_TEXTURE_MAX_RESOLUTION_HEIGHT] = std::to_string(param.maxSize.y);
    node.attributes_[CSTR_TEXTURE_AMBIENT]               = Utils::BoolToString(param.useAmbient);
    node.attributes_[CSTR_TEXTURE_DIFFUSE]               = Utils::BoolToString(param.useDiffuse);
    node.attributes_[CSTR_TEXTURE_NORMAL]                = Utils::BoolToString(param.useNormal);
    node.attributes_[CSTR_TEXTURE_SPECULAR]              = Utils::BoolToString(param.useSpecular);
}
void Create(XmlNode& node, const Params::Particles& param)
{
    node.attributes_[CSTR_PARTICLES_ENABLED] = Utils::BoolToString(param.useParticles);
}
void Create(XmlNode& node, const Params::Shadows& param)
{
    node.attributes_[CSTR_SHADOWS_ENABLED]       = Utils::BoolToString(param.isEnabled);
    node.attributes_[CSTR_SHADOWS_VIEW_DISTANCE] = std::to_string(param.distance);
    node.attributes_[CSTR_SHADOWS_MAP_SIZE]      = std::to_string(param.mapSize);
}
void Create(XmlNode& node, const Params::Flora& param)
{
    node.attributes_[CSTR_FLORA_ENABLED]       = Utils::BoolToString(param.isEnabled);
    node.attributes_[CSTR_FLORA_GRASS]         = std::to_string(param.isGrass);
    node.attributes_[CSTR_FLORA_VIEW_DISTANCE] = std::to_string(param.viewDistance);
}
void Create(XmlNode& node, const Params::Water& param)
{
    node.attributes_[CSTR_WATER_TYPE] = std::to_string(static_cast<uint32>(param.type));

    auto& reflection                                     = node.AddChild(CSTR_WATER_REFLECTION);
    reflection.attributes_[CSTR_WATER_REFLECTION_WIDTH]  = std::to_string(param.waterReflectionResolution.x);
    reflection.attributes_[CSTR_WATER_REFLECTION_HEIGHT] = std::to_string(param.waterReflectionResolution.y);

    auto& refraction                                     = node.AddChild(CSTR_WATER_REFRACTION);
    refraction.attributes_[CSTR_WATER_REFRACTION_WIDTH]  = std::to_string(param.waterRefractionResolution.x);
    refraction.attributes_[CSTR_WATER_REFRACTION_HEIGHT] = std::to_string(param.waterRefractionResolution.y);
}
void Create(XmlNode& node, const Params::Renderer& param)
{
    node.attributes_[CSTR_RENDERER_TYPE]             = std::to_string(static_cast<uint32>(param.type));
    node.attributes_[CSTR_RENDERER_FPS_RESOLUTION_X] = std::to_string(param.resolution.x);
    node.attributes_[CSTR_RENDERER_FPS_RESOLUTION_Y] = std::to_string(param.resolution.y);
    node.attributes_[CSTR_RENDERER_FPS_LIMIT]        = std::to_string(param.fpsLimt);
    Create(node.AddChild(CSTR_WATER), param.water);
    Create(node.AddChild(CSTR_FLORA), param.flora);
    Create(node.AddChild(CSTR_SHADOWS), param.shadows);
    Create(node.AddChild(CSTR_TEXTURES), param.textures);
    Create(node.AddChild(CSTR_PARTICLES), param.particles);
}
void Create(XmlNode& node, const Params::Window& param)
{
    node.attributes_[CSTR_WINDOW_NAME]       = param.name;
    node.attributes_[CSTR_WINDOW_WIDTH]      = std::to_string(param.size.x);
    node.attributes_[CSTR_WINDOW_HEIGHT]     = std::to_string(param.size.y);
    node.attributes_[CSTR_WINDOW_FULLSCREEN] = Utils::BoolToString(param.fullScreen);
}
void Create(XmlNode& node, const Params::Files& param)
{
    node.AddChild(CSTR_DATA_LOCATION).value_        = param.data;
    node.AddChild(CSTR_SHADER_LOCATION).value_      = param.shaders;
    node.AddChild(CSTR_REQUIRED_FILE_OUTPUT).value_ = param.requiredFilesOutputFile;
}
void Create(XmlNode& node, const Params::Sound& param)
{
    node.attributes_[CSTR_SOUND_ENABLED] = std::to_string(param.isEnabled);
    node.attributes_[CSTR_SOUND_VOLUME]  = std::to_string(param.volume);
}
void CreateBinary(XmlNode& node, bool param)
{
    node.value_ = Utils::BoolToString(param);
}
void WriteConfigurationToFile(const Configuration& configuration, const std::string& filename)
{
    XmlNode config(CSTR_CONFIG);

    Create(config.AddChild(CSTR_WINDOW), configuration.window);
    Create(config.AddChild(CSTR_FILES), configuration.files);
    Create(config.AddChild(CSTR_SOUND), configuration.sound);
    Create(config.AddChild(CSTR_RENDERER), configuration.renderer);
    CreateBinary(config.AddChild(CSTR_ENABLE_BINARY_LOADING), configuration.useBinaryLoading);

    Xml::Write(filename, config);
}
}  // namespace GameEngine
