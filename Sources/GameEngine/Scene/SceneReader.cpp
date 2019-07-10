#include "SceneReader.h"
#include "Logger/Log.h"
#include "Scene.hpp"
#include "SceneDef.h"
#include "Utils.h"
#include "Utils/XML/XMLUtils.h"
#include "Utils/XML/XmlReader.h"

#include "GameEngine/Engine/Configuration.h"
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
#include "GameEngine/Components/Renderer/Terrain/TerrainDef.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Components/Renderer/Trees/TreeRendererComponent.h"
#include "GameEngine/Components/Renderer/Water/WaterRendererComponent.h"

using namespace Utils;

namespace GameEngine
{
namespace SceneReader
{
namespace
{
Scene* currentReadingScene = nullptr;
}

std::vector<vec3> ReadVectorVec3(Utils::XmlNode& node)
{
    std::vector<vec3> result;
    for (const auto& v : node.GetChildren())
    {
        result.emplace_back(std::stof(v->attributes_[CSTR_X]), std::stof(v->attributes_[CSTR_Y]),
                            std::stof(v->attributes_[CSTR_Z]));
    }
    return result;
}

void Read(Utils::XmlNode& node, common::Transform& tranfsorm)
{
    auto position = ReadVec3(*node.GetChild(CSTR_POSITION));
    tranfsorm.SetPosition(position);
    auto rotation = ReadVec3(*node.GetChild(CSTR_ROTATION));
    tranfsorm.SetRotation(rotation);
    auto scale = ReadVec3(*node.GetChild(CSTR_SCALE));
    tranfsorm.SetScale(scale);

    tranfsorm.isDynamic_ = ReadBool(*node.GetChild(CSTR_ISDYNAMIC));
    tranfsorm.TakeSnapShoot();
}

void Read(Utils::XmlNode&, Components::ThridPersonCameraComponent&)
{
}

void Read(Utils::XmlNode& node, Components::Animator& component)
{
    component.SetAnimation(node.GetChild(CSTR_CURRENT_ANIMATION)->value_);
}

void Read(Utils::XmlNode& node, Components::BoxShape& component)
{
    component.SetSize(ReadVec3(*node.GetChild(CSTR_SIZE)));
    component.SetPostionOffset(ReadVec3(*node.GetChild(CSTR_POSITION_OFFSET)));
}

void Read(Utils::XmlNode& node, Components::MeshShape& component)
{
    component.SetSize(ReadFloat(*node.GetChild(CSTR_SIZE)));
    component.SetPostionOffset(ReadVec3(*node.GetChild(CSTR_POSITION_OFFSET)));

    component.SetModel(node.GetChild(CSTR_MODEL_FILE_NAME)->value_);
}

void Read(Utils::XmlNode& node, Components::SphereShape& component)
{
    component.SetSize(ReadFloat(*node.GetChild(CSTR_SIZE)));
    component.SetPostionOffset(ReadVec3(*node.GetChild(CSTR_POSITION_OFFSET)));
}

void Read(Utils::XmlNode& node, Components::CapsuleShape& component)
{
    component.SetHeight(ReadFloat(*node.GetChild(CSTR_HEIGHT)));
    component.SetRadius(ReadFloat(*node.GetChild(CSTR_RADIUS)));
    component.SetPostionOffset(ReadVec3(*node.GetChild(CSTR_POSITION_OFFSET)));
}

void Read(Utils::XmlNode& node, Components::TerrainShape& component)
{
    component.SetPostionOffset(ReadVec3(*node.GetChild(CSTR_POSITION_OFFSET)));
    component.SetHeightMap(node.GetChild(CSTR_HEIGHTMAP_FILENAME)->value_);
}

void Read(Utils::XmlNode& node, Components::Rigidbody& component)
{
    component.SetMass(ReadFloat(*node.GetChild(CSTR_MASS)));
    component.SetIsStatic(ReadBool(*node.GetChild(CSTR_IS_STATIC)));
    component.SetVelocity(ReadVec3(*node.GetChild(CSTR_VELOCITY)));

    auto angularFactorNode = node.GetChild(CSTR_ANGULAR_FACTOR);

    if (angularFactorNode)
        component.SetAngularFactor(ReadFloat(*angularFactorNode));

    auto collShape = static_cast<Components::ComponentsType>(std::stoi(node.GetChild(CSTR_COLLISION_SHAPE)->value_));
    component.SetCollisionShape(collShape);
}

void Read(Utils::XmlNode& node, Components::RendererComponent& component)
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

void Read(Utils::XmlNode& node, Components::TreeRendererComponent& component)
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

Particle ReadParticle(Utils::XmlNode& node)
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

void Read(Utils::XmlNode& node, Components::ParticleEffectComponent& component)
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

std::vector<std::string> ReadStringVector(Utils::XmlNode& node, const std::string& str)
{
    std::vector<std::string> textures;
    for (const auto& modelFileName : node.GetChild(str)->GetChildren())
    {
        textures.push_back(modelFileName->value_);
    }
    return textures;
}

std::vector<float> ReadFloatVector(Utils::XmlNode& node)
{
    auto strings = Utils::SplitString(node.value_, ' ');

    std::vector<float> v;
    v.reserve(strings.size());
    for (const auto& value : strings)
    {
        v.push_back(std::stof(value));
    }
    return v;
}

void Read(Utils::XmlNode& node, Components::SkyBoxComponent& component)
{
    component.SetDayTexture(ReadStringVector(node, CSTR_DAY_TEXTURES));
    component.SetNightTexture(ReadStringVector(node, CSTR_NIGHT_TEXTURES));
    component.SetModel(node.GetChild(CSTR_MODEL_FILE_NAME)->value_);
}

void Read(Utils::XmlNode&, Components::SkydomeComponent&)
{
}

void Read(Utils::XmlNode&, Components::CharacterController&)
{
}

void Read(Utils::XmlNode&, Components::PlayerInputController&)
{
}

void Read(Utils::XmlNode& node, Components::WaterRendererComponent& component)
{
    auto waveSpeedNode = node.GetChild(CSTR_WAVE_SPEED);
    if (waveSpeedNode)
        component.SetWaveSpeed(ReadFloat(*waveSpeedNode));

    auto colorNode = node.GetChild(CSTR_COLOR);
    if (colorNode)
        component.SetWaterColor(ReadVec4(*colorNode));

    auto positionNode = node.GetChild(CSTR_POSITION);
    if (positionNode)
        component.SetPosition(ReadVec3(*positionNode));

    auto scaleNode = node.GetChild(CSTR_SCALE);
    if (scaleNode)
        component.SetScale(ReadVec3(*scaleNode));

    auto dudvNode      = node.GetChild(CSTR_DUDV_MAP);
    auto normalMapNode = node.GetChild(CSTR_NORMAL_MAP);
    if (dudvNode and normalMapNode)
        component.LoadTextures(dudvNode->value_, normalMapNode->value_);
}

std::unordered_map<TerrainTextureType, std::string> ReadTerrainTextures(Utils::XmlNode& node)
{
    std::unordered_map<TerrainTextureType, std::string> result;

    for (const auto& texture : node.GetChildren())
    {
        const auto& filename = texture->GetChild(CSTR_TEXTURE_FILENAME)->value_;
        auto type            = static_cast<TerrainTextureType>(std::stoi(texture->GetChild(CSTR_TEXTURE_TYPE)->value_));
        result.insert({type, filename});
    }

    return result;
}

void Read(Utils::XmlNode& node, Components::GrassRendererComponent& component)
{
    component.SetTexture(node.GetChild(CSTR_TEXTURE_FILENAME)->value_);
    component.SetPositions(ReadFloatVector(*node.GetChild(CSTR_POSITIONS)));
}

void Read(Utils::XmlNode& node, Components::TerrainRendererComponent& component)
{
    auto textures = ReadTerrainTextures(*node.GetChild(CSTR_TEXTURE_FILENAMES));
    component.LoadTextures(textures);
}

void Read(Utils::XmlNode& node, Components::TerrainMeshRendererComponent& component)
{
    auto textures = ReadTerrainTextures(*node.GetChild(CSTR_TEXTURE_FILENAMES));
    component.LoadTextures(textures);
}

template <typename T>
void AddComponent(Utils::XmlNode& node, GameObject& gameObject)
{
    auto& comp = gameObject.AddComponent<T>();
    Read(node, comp);
}

void Read(Utils::XmlNode& node, GameObject& gameObject)
{
    Read(*node.GetChild(CSTR_TRANSFORM), gameObject.worldTransform);

    auto& componentsNode = *node.GetChild(CSTR_COMPONENTS);

    for (const auto& component : componentsNode.GetChildren())
    {
        auto type = static_cast<Components::ComponentsType>(std::stoi(component->attributes_[CSTR_TYPE]));

        switch (type)
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
            case Components::ComponentsType::TerrainMeshRenderer:
                AddComponent<Components::TerrainMeshRendererComponent>(*component, gameObject);
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
}

void LoadPrefab(Scene& scene, const std::string& filename, const std::string& name)
{
    Utils::XmlReader xmlReader;

    if (not xmlReader.Read(EngineConf_GetFullDataPathAddToRequierd(filename)))
    {
        ERROR_LOG("Prefab read error");
        return;
    }

    currentReadingScene = &scene;
    DEBUG_LOG("filename : " + EngineConf_GetFullDataPathAddToRequierd(filename));
    DEBUG_LOG("Name : " + name);

    auto gameObject = scene.CreateGameObject(name);
    Read(*xmlReader.Get(CSTR_PREFAB), *gameObject);
    scene.AddGameObject(gameObject);
}

void Read(Utils::XmlNode& node, Scene& scene)
{
    for (const auto& child : node.GetChild(CSTR_GAMEOBJECTS)->GetChildren())
    {
        auto gameObject = scene.CreateGameObject(child->attributes_.at(CSTR_NAME));
        Read(*child, *gameObject);
        scene.AddGameObject(gameObject);
    }

    for (const auto& child : node.GetChild(CSTR_PREFABS)->GetChildren())
    {
        LoadPrefab(scene, child->value_, child->attributes_.at(CSTR_NAME));
    }
}

void LoadScene(Scene& scene, const std::string& filename)
{
    Utils::XmlReader xmlReader;
    if (!xmlReader.Read(filename))
        return;

    currentReadingScene = &scene;
    Read(*xmlReader.Get(CSTR_SCENE), scene);
}
}  // namespace SceneReader
}  // namespace GameEngine
