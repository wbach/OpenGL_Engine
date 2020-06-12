#include "SceneReader.h"

#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Camera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Controllers/CharacterController.h"
#include "GameEngine/Components/Input/PlayerInputController.h"
#include "GameEngine/Components/Physics/BoxShape.h"
#include "GameEngine/Components/Physics/CapsuleShape.h"
#include "GameEngine/Components/Physics/MeshShape.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Physics/SphereShape.h"
#include "GameEngine/Components/Physics/Terrain/TerrainShape.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/Grass/GrassComponent.h"
#include "GameEngine/Components/Renderer/Particles/ParticleEffectComponent.h"
#include "GameEngine/Components/Renderer/SkyBox/SkyBoxComponent.h"
#include "GameEngine/Components/Renderer/Skydome/SkydomeComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Components/Renderer/Trees/TreeRendererComponent.h"
#include "GameEngine/Components/Renderer/Water/WaterRendererComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "Logger/Log.h"
#include "Scene.hpp"
#include "SceneDef.h"
#include "Utils.h"
#include "Utils/XML/XMLUtils.h"
#include "Utils/XML/XmlReader.h"

using namespace Utils;

namespace GameEngine
{
namespace SceneReader
{
namespace
{
Scene* currentReadingScene = nullptr;
}

std::vector<vec3> ReadVectorVec3(const Utils::XmlNode& node)
{
    std::vector<vec3> result;
    for (const auto& v : node.GetChildren())
    {
        result.emplace_back(std::stof(v->attributes_[CSTR_X]), std::stof(v->attributes_[CSTR_Y]),
                            std::stof(v->attributes_[CSTR_Z]));
    }
    return result;
}

void Read(const Utils::XmlNode& node, common::Transform& tranfsorm)
{
    auto position = ReadVec3(*node.GetChild(CSTR_POSITION));
    auto rotation = ReadVec3(*node.GetChild(CSTR_ROTATION));
    auto scale    = ReadVec3(*node.GetChild(CSTR_SCALE));

    tranfsorm.SetPositionAndRotationAndScale(position, DegreesVec3(rotation), scale);
}

void Read(const Utils::XmlNode&, Components::ThridPersonCameraComponent&)
{
}

void Read(const Utils::XmlNode& node, Components::Animator& component)
{
    component.SetAnimation(node.GetChild(CSTR_CURRENT_ANIMATION)->value_);
}

void Read(const Utils::XmlNode& node, Components::BoxShape& component)
{
    component.SetSize(ReadVec3(*node.GetChild(CSTR_SIZE)));
    component.SetPostionOffset(ReadVec3(*node.GetChild(CSTR_POSITION_OFFSET)));
}

void Read(const Utils::XmlNode& node, Components::MeshShape& component)
{
    component.SetSize(ReadFloat(*node.GetChild(CSTR_SIZE)));
    component.SetPostionOffset(ReadVec3(*node.GetChild(CSTR_POSITION_OFFSET)));

    component.SetModel(node.GetChild(CSTR_MODEL_FILE_NAME)->value_);
}

void Read(const Utils::XmlNode& node, Components::SphereShape& component)
{
    component.SetSize(ReadFloat(*node.GetChild(CSTR_SIZE)));
    component.SetPostionOffset(ReadVec3(*node.GetChild(CSTR_POSITION_OFFSET)));
}

void Read(const Utils::XmlNode& node, Components::CapsuleShape& component)
{
    component.SetHeight(ReadFloat(*node.GetChild(CSTR_HEIGHT)));
    component.SetRadius(ReadFloat(*node.GetChild(CSTR_RADIUS)));
    component.SetPostionOffset(ReadVec3(*node.GetChild(CSTR_POSITION_OFFSET)));
}

void Read(const Utils::XmlNode& node, Components::TerrainShape& component)
{
    component.SetPostionOffset(ReadVec3(*node.GetChild(CSTR_POSITION_OFFSET)));
    component.SetHeightMap(node.GetChild(CSTR_HEIGHTMAP_FILENAME)->value_);
}

void Read(const Utils::XmlNode& node, Components::Rigidbody& component)
{
    component.SetMass(ReadFloat(*node.GetChild(CSTR_MASS)));
    component.SetIsStatic(ReadBool(*node.GetChild(CSTR_IS_STATIC)));

    auto velocityNode = node.GetChild(CSTR_VELOCITY);
    if (velocityNode)
        component.InputParams().velocity_ = ReadVec3(*velocityNode);

    auto angularFactorNode = node.GetChild(CSTR_ANGULAR_FACTOR);
    if (angularFactorNode)
    {
        if (angularFactorNode->value_.empty())
            component.InputParams().angularFactor_ = ReadVec3(*angularFactorNode);
        else
            component.InputParams().angularFactor_ = vec3(ReadFloat(*angularFactorNode));
    }

    auto collShape = static_cast<Components::ComponentsType>(std::stoi(node.GetChild(CSTR_COLLISION_SHAPE)->value_));
    component.SetCollisionShape(collShape);
}

void Read(const Utils::XmlNode& node, Components::RendererComponent& component)
{
    auto textureIndex = std::stoul(node.GetChild(CSTR_TEXTURE_INDEX)->value_);
    component.SetTextureIndex(textureIndex);

    for (const auto& fileNode : node.GetChild(CSTR_MODEL_FILE_NAMES)->GetChildren())
    {
        const auto& filename = fileNode->GetChild(CSTR_FILE_NAME)->value_;
        auto lod = static_cast<LevelOfDetail>(std::stoi(fileNode->GetChild(CSTR_MODEL_LVL_OF_DETAIL)->value_));
        component.AddModel(filename, lod);
    }
}

void Read(const Utils::XmlNode& node, Components::TreeRendererComponent& component)
{
    for (const auto& fileNode : node.GetChild(CSTR_BOTTOM_FILENAMES)->GetChildren())
    {
        const auto& filename = fileNode->GetChild(CSTR_FILE_NAME)->value_;
        auto lod = static_cast<LevelOfDetail>(std::stoi(fileNode->GetChild(CSTR_MODEL_LVL_OF_DETAIL)->value_));
        component.SetBottomModel(filename, lod);
    }
    for (const auto& fileNode : node.GetChild(CSTR_TOP_FILENAMES)->GetChildren())
    {
        const auto& filename = fileNode->GetChild(CSTR_FILE_NAME)->value_;
        auto lod = static_cast<LevelOfDetail>(std::stoi(fileNode->GetChild(CSTR_MODEL_LVL_OF_DETAIL)->value_));
        component.SetTopModel(filename, lod);
    }

    component.SetPositions(ReadVectorVec3(*node.GetChild(CSTR_POSITIONS)), ReadVec2ui(*node.GetChild(CSTR_SIZE_2D)));
}

Particle ReadParticle(const Utils::XmlNode& node)
{
    Particle particle;

    particle.position      = ReadVec3(*node.GetChild(CSTR_POSITION));
    particle.rotation      = std::stof(node.GetChild(CSTR_ROTATION)->value_);
    particle.scale         = std::stof(node.GetChild(CSTR_SCALE)->value_);
    particle.velocity      = ReadVec3(*node.GetChild(CSTR_VELOCITY));
    particle.gravityEffect = ReadBool(*node.GetChild(CSTR_GRAVITY_EFFECT));
    particle.lifeTime      = std::stof(node.GetChild(CSTR_LIFE_TIME)->value_);

    return particle;
}

void Read(const Utils::XmlNode& node, Components::ParticleEffectComponent& component)
{
    auto particle = ReadParticle(*node.GetChild(CSTR_PARTICLE));
    component.SetParticle(particle);
    component.SetTexture(node.GetChild(CSTR_TEXTURE)->value_);
    component.SetParticlesPerSec(std::stoul(node.GetChild(CSTR_PARTICLE_PER_SER)->value_));
    component.SetBlendFunction(
        static_cast<GraphicsApi::BlendFunctionType>(std::stoi(node.GetChild(CSTR_BLEND_TYPE)->value_)));

    auto emitFunctionName = node.GetChild(CSTR_EMIT_FUNCTION)->value_;
    auto emitFunction     = currentReadingScene->GetParticleEmitFunction(emitFunctionName);
    if (emitFunction)
    {
        component.SetEmitFunction(emitFunctionName, *emitFunction);
    }
    component.SetSpeed(std::stof(node.GetChild(CSTR_SPEED)->value_));
    auto animated = Utils::StringToBool(node.GetChild(CSTR_IS_ANIMATED)->value_);

    if (animated)
    {
        component.EnableAnimation();
    }
    component.SetParticlesLimit(std::stoul(node.GetChild(CSTR_PARTICLE_LIMT)->value_));
}

std::vector<std::string> ReadStringVector(const Utils::XmlNode& node, const std::string& str)
{
    std::vector<std::string> textures;
    for (const auto& modelFileName : node.GetChild(str)->GetChildren())
    {
        textures.push_back(modelFileName->value_);
    }
    return textures;
}

std::array<File, 6> ReadCubeMapArray(const Utils::XmlNode& node, const std::string& str)
{
    std::array<File, 6> textures;
    uint32 index = 0;
    for (const auto& modelFileName : node.GetChild(str)->GetChildren())
    {
        if (index < 6)
        {
            textures[index++] = modelFileName->value_;
        }
        else
        {
            ERROR_LOG("To many textures in cubeMap texture.");
        }
    }
    return textures;
}

std::vector<float> ReadFloatVector(const Utils::XmlNode& node)
{
    auto strings = Utils::SplitString(node.value_, ' ');
    DEBUG_LOG("string size : " + std::to_string(strings.size()));
    std::vector<float> v;
    v.reserve(strings.size());
    for (const auto& value : strings)
    {
        try
        {
            v.push_back(std::stof(value));
        }
        catch (...)
        {
            DEBUG_LOG("stof exceptrion : " + value);
        }
    }
    return v;
}

void Read(const Utils::XmlNode& node, Components::SkyBoxComponent& component)
{
    component.SetDayTexture(ReadCubeMapArray(node, CSTR_DAY_TEXTURES));
    component.SetNightTexture(ReadCubeMapArray(node, CSTR_NIGHT_TEXTURES));
    component.SetModel(node.GetChild(CSTR_MODEL_FILE_NAME)->value_);
}

void Read(const Utils::XmlNode&, Components::SkydomeComponent&)
{
}

void Read(const Utils::XmlNode&, Components::CharacterController&)
{
}

void Read(const Utils::XmlNode& node, Components::PlayerInputController& component)
{
    auto animationClipsNode = node.GetChild(CSTR_ANIMATION_CLIPS);
    if (animationClipsNode)
    {
        if (auto animationNode = node.GetChild(CSTR_IDLE_ANIMATION))
        {
            component.idleAnimationName_ = animationNode->value_;
        }
        if (auto animationNode = node.GetChild(CSTR_WALK_ANIMATION))
        {
            component.walkAnimationName_ = animationNode->value_;
        }
        if (auto animationNode = node.GetChild(CSTR_RUN_ANIMATION))
        {
            component.runAnimationName_ = animationNode->value_;
        }
    }
}

void Read(const Utils::XmlNode& node, Components::WaterRendererComponent& component)
{
    auto waveSpeedNode = node.GetChild(CSTR_WAVE_SPEED);
    if (waveSpeedNode)
        component.SetWaveSpeed(ReadFloat(*waveSpeedNode));

    auto colorNode = node.GetChild(CSTR_COLOR);
    if (colorNode)
        component.SetWaterColor(ReadVec4(*colorNode));

    auto dudvNode      = node.GetChild(CSTR_DUDV_MAP);
    auto normalMapNode = node.GetChild(CSTR_NORMAL_MAP);
    if (dudvNode and normalMapNode)
        component.LoadTextures(dudvNode->value_, normalMapNode->value_);
}

std::vector<Components::TerrainComponentBase::TerrainTexture> ReadTerrainTextures(const Utils::XmlNode& node)
{
    std::vector<Components::TerrainComponentBase::TerrainTexture> result;

    for (const auto& texture : node.GetChildren())
    {
        Components::TerrainComponentBase::TerrainTexture terrainTexture;
        if (texture->GetChild(CSTR_TEXTURE_FILENAME))
            terrainTexture.file = File(texture->GetChild(CSTR_TEXTURE_FILENAME)->value_);

        if (texture->GetChild(CSTR_TEXTURE_TYPE))
            std::from_string(texture->GetChild(CSTR_TEXTURE_TYPE)->value_, terrainTexture.type);
        else
            WARNING_LOG("Read texture without type");

        if (texture->GetChild(CSTR_SCALE))
            terrainTexture.tiledScale = ReadFloat(*texture->GetChild(CSTR_SCALE));

        result.push_back(terrainTexture);
    }
    std::sort(result.begin(), result.end(),
              [](const auto& l, const auto& r) { return static_cast<int>(l.type) < static_cast<int>(r.type); });
    return result;
}

void Read(const Utils::XmlNode& node, Components::GrassRendererComponent& component)
{
    component.setTexture(node.GetChild(CSTR_TEXTURE_FILENAME)->value_);

    if (node.GetChild(CSTR_FILE_NAME))
    {
        Components::GrassRendererComponent::GrassMeshes meshesData;

        auto filename = node.GetChild(CSTR_FILE_NAME)->value_;

        if (not filename.empty())
        {
            File file(filename);
            auto succes = file.openToRead();
            if (succes)
            {
                file.readVectorFromFile(meshesData.positions);
                file.readVectorFromFile(meshesData.normals);
                file.readVectorFromFile(meshesData.colors);
                file.readVectorFromFile(meshesData.sizesAndRotations);
                file.close();
            }
        }
        component.SetMeshesData(std::move(meshesData));
    }
}

void Read(const Utils::XmlNode& node, Components::TerrainRendererComponent& component)
{
    auto texturesNode = node.GetChild(CSTR_TEXTURE_FILENAMES);

    if (texturesNode)
    {
        auto textures = ReadTerrainTextures(*node.GetChild(CSTR_TEXTURE_FILENAMES));
        component.LoadTextures(textures);
    }
    else
    {
        ERROR_LOG("Child node with textures not found in terrain render component.");
    }
}

template <typename T>
void AddComponent(const Utils::XmlNode& node, GameObject& gameObject)
{
    auto& comp = gameObject.AddComponent<T>();
    Read(node, comp);
}

void Read(Scene& scene, const Utils::XmlNode& node, GameObject& gameObject)
{
    Read(*node.GetChild(CSTR_TRANSFORM), gameObject.GetTransform());

    auto& componentsNode = *node.GetChild(CSTR_COMPONENTS);

    for (const auto& component : componentsNode.GetChildren())
    {
        auto type = Components::from_string(component->attributes_[CSTR_TYPE]);

        if (not type)
            continue;

        switch (*type)
        {
            case Components::ComponentsType::Animator:
                AddComponent<Components::Animator>(*component, gameObject);
                break;
            case Components::ComponentsType::BoxShape:
                AddComponent<Components::BoxShape>(*component, gameObject);
                break;
            case Components::ComponentsType::MeshShape:
                AddComponent<Components::MeshShape>(*component, gameObject);
                break;
            case Components::ComponentsType::SphereShape:
                AddComponent<Components::SphereShape>(*component, gameObject);
                break;
            case Components::ComponentsType::TerrainShape:
                AddComponent<Components::TerrainShape>(*component, gameObject);
                break;
            case Components::ComponentsType::CapsuleShape:
                AddComponent<Components::CapsuleShape>(*component, gameObject);
                break;
            case Components::ComponentsType::Rigidbody:
                AddComponent<Components::Rigidbody>(*component, gameObject);
                break;
            case Components::ComponentsType::Renderer:
                AddComponent<Components::RendererComponent>(*component, gameObject);
                break;
            case Components::ComponentsType::TreeRenderer:
                AddComponent<Components::TreeRendererComponent>(*component, gameObject);
                break;
            case Components::ComponentsType::ParticleEffect:
                AddComponent<Components::ParticleEffectComponent>(*component, gameObject);
                break;
            case Components::ComponentsType::SkyBox:
                AddComponent<Components::SkyBoxComponent>(*component, gameObject);
                break;
            case Components::ComponentsType::Skydome:
                AddComponent<Components::SkydomeComponent>(*component, gameObject);
                break;
            case Components::ComponentsType::Grass:
                AddComponent<Components::GrassRendererComponent>(*component, gameObject);
                break;
            case Components::ComponentsType::TerrainRenderer:
                AddComponent<Components::TerrainRendererComponent>(*component, gameObject);
                break;
            case Components::ComponentsType::Water:
                AddComponent<Components::WaterRendererComponent>(*component, gameObject);
                break;
            case Components::ComponentsType::ThridPersonCamera:
                AddComponent<Components::ThridPersonCameraComponent>(*component, gameObject);
                break;
            case Components::ComponentsType::CharacterController:
                AddComponent<Components::CharacterController>(*component, gameObject);
                break;
            case Components::ComponentsType::PlayerInputController:
                AddComponent<Components::PlayerInputController>(*component, gameObject);
                break;
        }
    }
    auto childrenNode = node.GetChild(CSTR_CHILDREN);
    if (childrenNode)
    {
        for (auto& childNode : childrenNode->GetChildren())
        {
            auto child = scene.CreateGameObject(childNode->attributes_.at(CSTR_NAME));
            Read(scene, *childNode, *child);
            gameObject.AddChild(std::move(child));
        }
    }
}

GameObject* loadPrefab(Scene& scene, const File& file, const std::string& name)
{
    Utils::XmlReader xmlReader;
    if (not xmlReader.Read(file.GetAbsoultePath()))
    {
        ERROR_LOG("Prefab read error");
        return nullptr;
    }

    currentReadingScene = &scene;
    DEBUG_LOG("Name : " + name);

    auto gameObject = scene.CreateGameObject(name);
    Read(scene, *xmlReader.Get(CSTR_PREFAB), *gameObject);

    auto result = gameObject.get();
    scene.AddGameObject(std::move(gameObject));
    return result;
}
std::unique_ptr<GameObject> createGameObject(const Utils::XmlNode& node, Scene& scene)
{
    if (node.attributes_.count(CSTR_ID))
    {
        uint32 id = std::stoi(node.attributes_.at(CSTR_ID));
        return scene.CreateGameObject(node.attributes_.at(CSTR_NAME), id);
    }

    return scene.CreateGameObject(node.attributes_.at(CSTR_NAME));
}
void readNode(const Utils::XmlNode& node, Scene& scene)
{
    for (const auto& child : node.GetChild(CSTR_GAMEOBJECTS)->GetChildren())
    {
        auto gameObject = createGameObject(*child, scene);
        Read(scene, *child, *gameObject);
        scene.AddGameObject(std::move(gameObject));
    }
}

void loadScene(Scene& scene, const File& file)
{
    Utils::XmlReader xmlReader;
    if (not xmlReader.Read(file.GetAbsoultePath()))
        return;

    currentReadingScene = &scene;
    readNode(*xmlReader.Get(CSTR_SCENE), scene);
}
}  // namespace SceneReader
}  // namespace GameEngine
