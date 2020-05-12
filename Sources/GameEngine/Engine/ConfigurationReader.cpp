#include "ConfigurationReader.h"

#include "Configuration.h"
#include "EngineDef.h"
#include "Utils.h"
#include "Utils/XML/XmlReader.h"
#include <Logger/Log.h>

namespace GameEngine
{
void Read(Utils::XmlNode& node, Params::Window& window)
{
    window.name       = node.attributes_[CSTR_WINDOW_NAME];
    window.size.x     = Utils::StringToInt(node.attributes_[CSTR_WINDOW_WIDTH]);
    window.size.y     = Utils::StringToInt(node.attributes_[CSTR_WINDOW_HEIGHT]);
    window.fullScreen = Utils::StringToBool(node.attributes_[CSTR_WINDOW_FULLSCREEN]);
}

void Read(Utils::XmlNode& node, Params::Sound& sound)
{
    sound.isEnabled = Utils::StringToBool(node.attributes_[CSTR_SOUND_ENABLED]);
    sound.volume    = Utils::StringToFloat(node.attributes_[CSTR_SOUND_VOLUME]);
}

void Read(Utils::XmlNode& node, Params::Shadows& shadows)
{
    shadows.isEnabled = Utils::StringToBool(node.attributes_[CSTR_SHADOWS_ENABLED]);
    shadows.distance  = Utils::StringToFloat(node.attributes_[CSTR_SHADOWS_VIEW_DISTANCE]);
    shadows.mapSize   = Utils::StringToInt(node.attributes_[CSTR_SHADOWS_MAP_SIZE]);
}

void Read(Utils::XmlNode& node, Params::Particles& particles)
{
    particles.useParticles = Utils::StringToBool(node.attributes_[CSTR_PARTICLES_ENABLED]);
}

void Read(Utils::XmlNode& node, Params::Flora& flora)
{
    flora.isEnabled    = Utils::StringToBool(node.attributes_[CSTR_FLORA_ENABLED]);
    flora.isGrass      = Utils::StringToBool(node.attributes_[CSTR_FLORA_GRASS]);
    flora.viewDistance = Utils::StringToFloat(node.attributes_[CSTR_FLORA_VIEW_DISTANCE]);
}

void Read(Utils::XmlNode& node, Params::Water& water)
{
    water.type = static_cast<Params::WaterType>(Utils::StringToInt(node.attributes_[CSTR_WATER_TYPE]));

    water.waterReflectionResolution.x =
        Utils::StringToInt(node.GetChild(CSTR_WATER_REFLECTION)->attributes_[CSTR_WATER_REFLECTION_WIDTH]);
    water.waterReflectionResolution.y =
        Utils::StringToInt(node.GetChild(CSTR_WATER_REFLECTION)->attributes_[CSTR_WATER_REFLECTION_HEIGHT]);

    water.waterRefractionResolution.x =
        Utils::StringToInt(node.GetChild(CSTR_WATER_REFRACTION)->attributes_[CSTR_WATER_REFLECTION_WIDTH]);
    water.waterRefractionResolution.y =
        Utils::StringToInt(node.GetChild(CSTR_WATER_REFRACTION)->attributes_[CSTR_WATER_REFLECTION_HEIGHT]);
}

void Read(Utils::XmlNode& node, Params::Textures& textures)
{
    textures.maxSize.x   = Utils::StringToInt(node.attributes_[CSTR_TEXTURE_MAX_RESOLUTION_WIDTH]);
    textures.maxSize.y   = Utils::StringToInt(node.attributes_[CSTR_TEXTURE_MAX_RESOLUTION_HEIGHT]);
    textures.useAmbient  = Utils::StringToBool(node.attributes_[CSTR_TEXTURE_AMBIENT]);
    textures.useDiffuse  = Utils::StringToBool(node.attributes_[CSTR_TEXTURE_DIFFUSE]);
    textures.useNormal   = Utils::StringToBool(node.attributes_[CSTR_TEXTURE_NORMAL]);
    textures.useSpecular = Utils::StringToBool(node.attributes_[CSTR_TEXTURE_SPECULAR]);
}

void Read(Utils::XmlNode* node, Params::TerrainType& param)
{
    if (not node)
        return;

    std::from_string(node->value_, param);
    DEBUG_LOG(node->value_);
}

void Read(Utils::XmlNode* node, Params::Terrain& param)
{
    if (not node)
        return;

    Read(node->GetChild(CSTR_TERRAIN_RENDERER_TYPE), param.terrainType);
}  // namespace GameEngine

void Read(Utils::XmlNode& node, Params::Renderer& renderer)
{
    renderer.graphicsApi = node.attributes_[CSTR_GRAPHICS_API];
    renderer.type = static_cast<GraphicsApi::RendererType>(Utils::StringToInt(node.attributes_[CSTR_RENDERER_TYPE]));
    renderer.viewDistance = Utils::StringToFloat(node.attributes_[CSTR_RENDERER_VIEW_DISTANCE]);
    renderer.fpsLimt      = Utils::StringToInt(node.attributes_[CSTR_RENDERER_FPS_LIMIT]);
    renderer.resolution.x = Utils::StringToInt(node.attributes_[CSTR_RENDERER_FPS_RESOLUTION_X]);
    renderer.resolution.y = Utils::StringToInt(node.attributes_[CSTR_RENDERER_FPS_RESOLUTION_Y]);

    Read(node.GetChild(CSTR_TERRAIN), renderer.terrain);
    Read(*node.GetChild(CSTR_WATER), renderer.water);
    Read(*node.GetChild(CSTR_FLORA), renderer.flora);
    Read(*node.GetChild(CSTR_SHADOWS), renderer.shadows);
    Read(*node.GetChild(CSTR_TEXTURES), renderer.textures);
    Read(*node.GetChild(CSTR_PARTICLES), renderer.particles);
}

void Read(Utils::XmlNode& node, Params::Files& files)
{
    files.data                    = GetDataLocationFromString(node.GetChild(CSTR_DATA_LOCATION)->value_);
    files.shaders                 = GetShaderLocationFromString(node.GetChild(CSTR_SHADER_LOCATION)->value_);
    files.requiredFilesOutputFile = node.GetChild(CSTR_REQUIRED_FILE_OUTPUT)->value_;
}

void Read(Utils::XmlNode* node, Params::PhysicsVisualizatorParams& params)
{
    if (not node)
        return;

    if (node->attributes_.count(CSTR_USE_WORKER) > 0)
    {
        params.useWorkredToUpdatePhysicsVisualization_ = Utils::StringToBool(node->attributes_.at(CSTR_USE_WORKER));
    }

    if (node->attributes_.count(CSTR_REFRESH_STEP_DOWN) > 0)
    {
        params.refreshRateStepDown_ = static_cast<uint32>(Utils::StringToInt(node->attributes_.at(CSTR_REFRESH_STEP_DOWN)));
    }
}

void Read(Utils::XmlNode* node, Params::DebugParams& params)
{
    if (not node)
        return;

    Read(node->GetChild(CSTR_PHYSICS_VISUALIZATION_PARAMS), params.physicsVisualizator);
}

void ReadConfiguration(Configuration& configuration, const std::string& filename)
{
    Utils::XmlReader xmlReader;
    if (!xmlReader.Read(filename))
        return;

    Read(*xmlReader.Get(CSTR_WINDOW), configuration.window);
    Read(*xmlReader.Get(CSTR_SOUND), configuration.sound);
    Read(*xmlReader.Get(CSTR_FILES), configuration.files);
    Read(*xmlReader.Get(CSTR_RENDERER), configuration.renderer);
    Read(xmlReader.Get(CSTR_DEBUG_PARAMS), configuration.debugParams);
    EngineConf.useBinaryLoading = Utils::StringToBool(xmlReader.Get(CSTR_ENABLE_BINARY_LOADING)->value_);
}
}  // namespace GameEngine
