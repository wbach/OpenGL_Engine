#include "ConfigurationReader.h"

#include <Logger/Log.h>

#include "Configuration.h"
#include "EngineDef.h"
#include "Utils.h"
#include "Utils/XML/XmlReader.h"

namespace GameEngine
{
void SetParamIfExist(Params::Shadows::CascadeDistanceFunc& param, const Utils::Attributes& attributes,
                     const std::string& paramName)
{
    if (attributes.count(paramName))
    {
        if (attributes.at(paramName) == "linear")
        {
            param = Params::Shadows::CascadeDistanceFunc::linear;
        }
        else if (attributes.at(paramName) == "quadratic")
        {
            param = Params::Shadows::CascadeDistanceFunc::quadratic;
        }
        else if (attributes.at(paramName) == "exp")
        {
            param = Params::Shadows::CascadeDistanceFunc::exp;
        }
    }
}
void SetParamIfExist(bool& param, const Utils::Attributes& attributes, const std::string& paramName)
{
    if (attributes.count(paramName))
    {
        param = Utils::StringToBool(attributes.at(paramName));
    }
}
void SetParamIfExist(float& param, const Utils::Attributes& attributes, const std::string& paramName)
{
    if (attributes.count(paramName))
    {
        param = Utils::StringToFloat(attributes.at(paramName));
    }
}
void SetParamIfExist(uint32& param, const Utils::Attributes& attributes, const std::string& paramName)
{
    if (attributes.count(paramName))
    {
        param = Utils::StringToInt(attributes.at(paramName));
    }
}
void SetParamIfExist(std::string& param, const Utils::Attributes& attributes, const std::string& paramName)
{
    if (attributes.count(paramName))
    {
        param = attributes.at(paramName);
    }
}
void Read(Utils::XmlNode& node, Params::Window& window)
{
    SetParamIfExist(window.name, node.attributes_, CSTR_WINDOW_NAME);
    SetParamIfExist(window.size.x, node.attributes_, CSTR_WINDOW_WIDTH);
    SetParamIfExist(window.size.y, node.attributes_, CSTR_WINDOW_HEIGHT);
    SetParamIfExist(window.fullScreen, node.attributes_, CSTR_WINDOW_FULLSCREEN);
}

void Read(Utils::XmlNode& node, Params::Sound& sound)
{
    SetParamIfExist(sound.isEnabled, node.attributes_, CSTR_SOUND_ENABLED);
    SetParamIfExist(sound.volume, node.attributes_, CSTR_SOUND_VOLUME);
}

void Read(Utils::XmlNode& node, Params::Shadows& shadows)
{
    SetParamIfExist(shadows.isEnabled, node.attributes_, CSTR_SHADOWS_ENABLED);
    SetParamIfExist(shadows.distance, node.attributes_, CSTR_SHADOWS_VIEW_DISTANCE);
    SetParamIfExist(shadows.mapSize, node.attributes_, CSTR_SHADOWS_MAP_SIZE);
    SetParamIfExist(shadows.cascadesSize, node.attributes_, CSTR_CASCADE_SIZE);
    SetParamIfExist(shadows.firstCascadeDistance, node.attributes_, CSTR_CASCADE_FIRST_DISTANCE);
    SetParamIfExist(shadows.cascadeDistanceFunc, node.attributes_, CSTR_CASCADE_DISTANCE_FUNCTION);

    if (shadows.cascadesSize < 1)
    {
        shadows.cascadesSize = 1;
        ERROR_LOG("Cascade size must be set minimum to 1");
    }
    if (shadows.cascadesSize > Params::MAX_SHADOW_MAP_CASADES)
    {
        ERROR_LOG("Set to many cascades, limit is : " + std::to_string(Params::MAX_SHADOW_MAP_CASADES));
        shadows.cascadesSize = Params::MAX_SHADOW_MAP_CASADES;
    }
    if (shadows.firstCascadeDistance < 1.f)
    {
        shadows.firstCascadeDistance = 1.f;
    }

}

void Read(Utils::XmlNode& node, Params::Particles& particles)
{
    SetParamIfExist(particles.useParticles, node.attributes_, CSTR_PARTICLES_ENABLED);
}

void Read(Utils::XmlNode& node, Params::Flora& flora)
{
    SetParamIfExist(flora.isEnabled, node.attributes_, CSTR_FLORA_ENABLED);
    SetParamIfExist(flora.isGrass, node.attributes_, CSTR_FLORA_GRASS);
    SetParamIfExist(flora.viewDistance, node.attributes_, CSTR_FLORA_VIEW_DISTANCE);
}

void Read(Utils::XmlNode& node, Params::Water& water)
{
    if (node.attributes_.count(CSTR_WATER_TYPE))
        water.type = static_cast<Params::WaterType>(Utils::StringToInt(node.attributes_.at(CSTR_WATER_TYPE)));

    if (node.GetChild(CSTR_WATER_REFLECTION))
    {
        SetParamIfExist(water.waterReflectionResolution.x, node.GetChild(CSTR_WATER_REFLECTION)->attributes_,
                        CSTR_WATER_REFLECTION_WIDTH);
        SetParamIfExist(water.waterReflectionResolution.y, node.GetChild(CSTR_WATER_REFRACTION)->attributes_,
                        CSTR_WATER_REFLECTION_HEIGHT);
    }
    if (node.GetChild(CSTR_WATER_REFRACTION))
    {
        SetParamIfExist(water.waterRefractionResolution.x, node.GetChild(CSTR_WATER_REFRACTION)->attributes_,
                        CSTR_WATER_REFRACTION_WIDTH);
        SetParamIfExist(water.waterRefractionResolution.y, node.GetChild(CSTR_WATER_REFRACTION)->attributes_,
                        CSTR_WATER_REFRACTION_HEIGHT);
    }
}

void Read(Utils::XmlNode& node, Params::Textures& textures)
{
    SetParamIfExist(textures.maxSize.x, node.attributes_, CSTR_TEXTURE_MAX_RESOLUTION_WIDTH);
    SetParamIfExist(textures.maxSize.y, node.attributes_, CSTR_TEXTURE_MAX_RESOLUTION_HEIGHT);
    SetParamIfExist(textures.useAmbient, node.attributes_, CSTR_TEXTURE_AMBIENT);
    SetParamIfExist(textures.useDisplacement, node.attributes_, CSTR_TEXTURE_DISPLACEMENT);
    SetParamIfExist(textures.useDiffuse, node.attributes_, CSTR_TEXTURE_DIFFUSE);
    SetParamIfExist(textures.useNormal, node.attributes_, CSTR_TEXTURE_NORMAL);
    SetParamIfExist(textures.useSpecular, node.attributes_, CSTR_TEXTURE_SPECULAR);
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
    if (node.attributes_.count(CSTR_RENDERER_TYPE))
        renderer.type =
            static_cast<GraphicsApi::RendererType>(Utils::StringToInt(node.attributes_.at(CSTR_RENDERER_TYPE)));

    SetParamIfExist(renderer.graphicsApi, node.attributes_, CSTR_GRAPHICS_API);
    SetParamIfExist(renderer.viewDistance, node.attributes_, CSTR_RENDERER_VIEW_DISTANCE);
    SetParamIfExist(renderer.normalMappingDistance, node.attributes_, CSTR_RENDERER_NORMALMAPPING_DISTANCE);
    SetParamIfExist(renderer.fpsLimt, node.attributes_, CSTR_RENDERER_FPS_LIMIT);
    SetParamIfExist(renderer.resolution.x, node.attributes_, CSTR_RENDERER_FPS_RESOLUTION_X);
    SetParamIfExist(renderer.resolution.y, node.attributes_, CSTR_RENDERER_FPS_RESOLUTION_Y);

    Read(node.GetChild(CSTR_TERRAIN), renderer.terrain);
    Read(*node.GetChild(CSTR_WATER), renderer.water);
    Read(*node.GetChild(CSTR_FLORA), renderer.flora);
    Read(*node.GetChild(CSTR_SHADOWS), renderer.shadows);
    Read(*node.GetChild(CSTR_TEXTURES), renderer.textures);
    Read(*node.GetChild(CSTR_PARTICLES), renderer.particles);
}

void Read(Utils::XmlNode& node, Params::Files& files)
{
    files.data    = GetDataLocationFromString(node.GetChild(CSTR_DATA_LOCATION)->value_);
    files.shaders = GetShaderLocationFromString(node.GetChild(CSTR_SHADER_LOCATION)->value_);
    if (node.GetChild(CSTR_REQUIRED_FILE_OUTPUT))
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
        params.refreshRateStepDown_ =
            static_cast<uint32>(Utils::StringToInt(node->attributes_.at(CSTR_REFRESH_STEP_DOWN)));
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

    if (xmlReader.Get(CSTR_WINDOW))
        Read(*xmlReader.Get(CSTR_WINDOW), configuration.window);
    if (xmlReader.Get(CSTR_SOUND))
        Read(*xmlReader.Get(CSTR_SOUND), configuration.sound);
    if (xmlReader.Get(CSTR_FILES))
        Read(*xmlReader.Get(CSTR_FILES), configuration.files);
    if (xmlReader.Get(CSTR_RENDERER))
        Read(*xmlReader.Get(CSTR_RENDERER), configuration.renderer);
    if (xmlReader.Get(CSTR_DEBUG_PARAMS))
        Read(xmlReader.Get(CSTR_DEBUG_PARAMS), configuration.debugParams);

    if (xmlReader.Get(CSTR_ENABLE_BINARY_LOADING))
        EngineConf.useBinaryLoading = Utils::StringToBool(xmlReader.Get(CSTR_ENABLE_BINARY_LOADING)->value_);
}
}  // namespace GameEngine
