#include "ConfigurationWriter.h"

#include "Configuration.h"
#include "ConfigurationParams/ParamToString.h"
#include "EngineDef.h"
#include "Utils/XML/XmlWriter.h"

namespace std
{
string to_string(GameEngine::Params::Shadows::CascadeDistanceFunc func)
{
    if (func == GameEngine::Params::Shadows::CascadeDistanceFunc::linear)
    {
        return "linear";
    }
    else if (func == GameEngine::Params::Shadows::CascadeDistanceFunc::quadratic)
    {
        return "quadratic";
    }
    else if (func == GameEngine::Params::Shadows::CascadeDistanceFunc::exp)
    {
        return "exp";
    }

    return {};
}
}  // namespace std

namespace GameEngine
{
using namespace Utils;

void Create(TreeNode& node, const Params::Textures& param)
{
    node.attributes_[CSTR_TEXTURE_MAX_RESOLUTION_WIDTH]  = std::to_string(param.maxSize->x);
    node.attributes_[CSTR_TEXTURE_MAX_RESOLUTION_HEIGHT] = std::to_string(param.maxSize->y);
    node.attributes_[CSTR_TEXTURE_SIZE_LIMIT]            = Utils::BoolToString(param.limitTextureSize);
    node.attributes_[CSTR_TEXTURE_AMBIENT]               = Utils::BoolToString(param.useAmbient);
    node.attributes_[CSTR_TEXTURE_DISPLACEMENT]          = Utils::BoolToString(param.useDisplacement);
    node.attributes_[CSTR_TEXTURE_DIFFUSE]               = Utils::BoolToString(param.useDiffuse);
    node.attributes_[CSTR_TEXTURE_NORMAL]                = Utils::BoolToString(param.useNormal);
    node.attributes_[CSTR_TEXTURE_SPECULAR]              = Utils::BoolToString(param.useSpecular);
}
void Create(TreeNode& node, const Params::Particles& param)
{
    node.attributes_[CSTR_PARTICLES_ENABLED] = Utils::BoolToString(param.useParticles);
}
void Create(TreeNode& node, const Params::Shadows& param)
{
    node.attributes_[CSTR_SHADOWS_ENABLED]           = Utils::BoolToString(param.isEnabled);
    node.attributes_[CSTR_SHADOWS_VIEW_DISTANCE]     = std::to_string(param.distance);
    node.attributes_[CSTR_SHADOWS_MAP_SIZE]          = std::to_string(param.mapSize);
    node.attributes_[CSTR_CASCADE_SIZE]              = std::to_string(param.cascadesSize);
    node.attributes_[CSTR_CASCADE_FIRST_DISTANCE]    = std::to_string(param.firstCascadeDistance);
    node.attributes_[CSTR_CASCADE_DISTANCE_FUNCTION] = std::to_string(param.cascadeDistanceFunc);
}
void Create(TreeNode& node, const Params::Flora& param)
{
    node.attributes_[CSTR_FLORA_ENABLED]       = Utils::BoolToString(param.isEnabled);
    node.attributes_[CSTR_FLORA_VIEW_DISTANCE] = std::to_string(param.viewDistance);
}
void Create(TreeNode& node, const Params::Water& param)
{
    node.attributes_[CSTR_WATER_TYPE] = std::to_string(static_cast<uint32>(*param.type));

    auto& reflection                                     = node.addChild(CSTR_WATER_REFLECTION);
    reflection.attributes_[CSTR_WATER_REFLECTION_WIDTH]  = std::to_string(param.waterReflectionResolution->x);
    reflection.attributes_[CSTR_WATER_REFLECTION_HEIGHT] = std::to_string(param.waterReflectionResolution->y);

    auto& refraction                                     = node.addChild(CSTR_WATER_REFRACTION);
    refraction.attributes_[CSTR_WATER_REFRACTION_WIDTH]  = std::to_string(param.waterRefractionResolution->x);
    refraction.attributes_[CSTR_WATER_REFRACTION_HEIGHT] = std::to_string(param.waterRefractionResolution->y);
}
void Create(TreeNode& node, const Params::TerrainType& param)
{
    node.value_ = std::to_string(param);
}
void Create(TreeNode& node, const Params::Terrain& param)
{
    Create(node.addChild(CSTR_TERRAIN_RENDERER_TYPE), param.terrainType);
    node.addChild(CSTR_TERRAIN_MESH_RESOLUTION_DIVIDER, std::to_string(param.resolutionDivideFactor));
    node.addChild(CSTR_TERRAIN_MESH_PARTS_COUNT, std::to_string(param.meshPartsCount));
}
void Create(TreeNode& node, const Params::Renderer& param)
{
    node.attributes_[CSTR_GRAPHICS_API]                    = param.graphicsApi;
    node.attributes_[CSTR_RENDERER_TYPE]                   = std::to_string(static_cast<uint32>(*param.type));
    node.attributes_[CSTR_PRESET]                          = std::to_string(static_cast<uint32>(*param.preset));
    node.attributes_[CSTR_RENDERER_RESOLUTION_X]           = std::to_string(param.resolution->x);
    node.attributes_[CSTR_RENDERER_RESOLUTION_Y]           = std::to_string(param.resolution->y);
    node.attributes_[CSTR_RENDERER_FPS_LIMIT]              = std::to_string(param.fpsLimt);
    node.attributes_[CSTR_RENDERER_VIEW_DISTANCE]          = std::to_string(param.viewDistance);
    node.attributes_[CSTR_RENDERER_LOD0_DISTANCE]          = std::to_string(param.lodDistance0);
    node.attributes_[CSTR_RENDERER_LOD1_DISTANCE]          = std::to_string(param.lodDistance1);
    node.attributes_[CSTR_RENDERER_LOD2_DISTANCE]          = std::to_string(param.lodDistance2);
    node.attributes_[CSTR_RENDERER_NORMALMAPPING_DISTANCE] = std::to_string(param.normalMappingDistance);
    node.attributes_[CSTR_USE_ENTITY_INSTANCED_GROUPING]   = Utils::BoolToString(param.useInstanceRendering);

    Create(node.addChild(CSTR_TERRAIN), param.terrain);
    Create(node.addChild(CSTR_WATER), param.water);
    Create(node.addChild(CSTR_FLORA), param.flora);
    Create(node.addChild(CSTR_SHADOWS), param.shadows);
    Create(node.addChild(CSTR_TEXTURES), param.textures);
    Create(node.addChild(CSTR_PARTICLES), param.particles);
}
void Create(TreeNode& node, const Params::Window& param)
{
    node.attributes_[CSTR_WINDOW_NAME]       = param.name;
    node.attributes_[CSTR_WINDOW_WIDTH]      = std::to_string(param.size->x);
    node.attributes_[CSTR_WINDOW_HEIGHT]     = std::to_string(param.size->y);
    node.attributes_[CSTR_WINDOW_FULLSCREEN] = Utils::BoolToString(param.fullScreen);
}
void Create(TreeNode& node, const Params::Files& param)
{
    node.addChild(CSTR_DATA_LOCATION, param.data);
    node.addChild(CSTR_SHADER_LOCATION, param.shaders);
    node.addChild(CSTR_USER_DIR_LOCATION, param.userDir);
    node.addChild(CSTR_CACHE_LOCATION, param.cache);
    node.addChild(CSTR_REQUIRED_FILE_OUTPUT, param.requiredFilesOutputFile);
    node.addChild(CSTR_LOADING_SCREEN_BACKGROUND, param.loadingScreenBackgroundTexture);
    node.addChild(CSTR_LOADING_SCREEN_CIRCLE, param.loadingScreenCircleTexture);
}
void Create(TreeNode& node, const Params::Sound& param)
{
    node.attributes_[CSTR_SOUND_ENABLED] = std::to_string(param.isEnabled);
    node.attributes_[CSTR_SOUND_VOLUME]  = std::to_string(param.volume);
}
void Create(TreeNode& node, const Params::PhysicsVisualizatorParams& param)
{
    node.attributes_[CSTR_USE_WORKER]        = Utils::BoolToString(param.useWorkredToUpdatePhysicsVisualization_);
    node.attributes_[CSTR_REFRESH_STEP_DOWN] = std::to_string(param.refreshRateStepDown_);
}
void Create(TreeNode& node, LogginLvl param)
{
    node.value_ = Params::paramToString(param);
}
void Create(TreeNode& node, const Params::DebugParams& param)
{
    Create(node.addChild(CSTR_PHYSICS_VISUALIZATION_PARAMS), param.physicsVisualizator);
    Create(node.addChild(CSTR_LOGGING_LVL), param.logLvl);
    node.addChild(CSTR_SHOW_RENDER_INFO, Utils::BoolToString(param.showRenderInfo));
}
void CreateBinary(TreeNode& node, bool param)
{
    node.value_ = Utils::BoolToString(param);
}
void WriteConfigurationToFile(const Configuration& configuration)
{
    WriteConfigurationToFile(configuration, configuration.filename);
}
void WriteConfigurationToFile(const Configuration& configuration, const std::string& filename)
{
    TreeNode config(CSTR_CONFIG);

    Create(config.addChild(CSTR_WINDOW), configuration.window);
    Create(config.addChild(CSTR_FILES), configuration.files);
    Create(config.addChild(CSTR_SOUND), configuration.sound);
    Create(config.addChild(CSTR_RENDERER), configuration.renderer);
    Create(config.addChild(CSTR_DEBUG_PARAMS), configuration.debugParams);
    CreateBinary(config.addChild(CSTR_ENABLE_BINARY_LOADING), configuration.useBinaryLoading);

    Xml::Write(filename, config);
}
}  // namespace GameEngine
