#include "SceneReader.h"
#include "Scene.hpp"
#include "SceneDef.h"
#include "Utils.h"
#include "Utils/XML/XmlReader.h"

#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Physics/BoxShape.h"
#include "GameEngine/Components/Physics/MeshShape.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Physics/SphereShape.h"
#include "GameEngine/Components/Physics/Terrain/TerrainShape.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/Grass/GrassComponent.h"
#include "GameEngine/Components/Renderer/Particles/ParticleEffectComponent.h"
#include "GameEngine/Components/Renderer/SkyBox/SkyBoxComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainDef.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Components/Renderer/Trees/TreeRendererComponent.h"

namespace GameEngine
{
namespace
{
Scene* currentReadingScene = nullptr;
}
float ReadFloat(Utils::XmlNode& node)
{
    return std::stof(node.value_);
}
bool ReadBool(Utils::XmlNode& node)
{
    return Utils::StringToBool(node.value_);
}

vec3 ReadVec3(Utils::XmlNode& node)
{
    vec3 v;
    v.x = std::stof(node.attributes_[CSTR_X]);
    v.y = std::stof(node.attributes_[CSTR_Y]);
    v.z = std::stof(node.attributes_[CSTR_Z]);
    return v;
}

vec2ui ReadVec2ui(Utils::XmlNode& node)
{
    vec2ui v;
    v.x = std::stol(node.attributes_[CSTR_X]);
    v.y = std::stol(node.attributes_[CSTR_Y]);
    return v;
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
    component.SetBlendFunction(static_cast<GraphicsApi::BlendFunctionType>(std::stoi(node.GetChild(CSTR_BLEND_TYPE)->value_)));

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
    std::vector<float> v;
    for (const auto& modelFileName : node.GetChildren())
    {
        v.push_back(std::stof(modelFileName->value_));
    }
    return v;
}

void Read(Utils::XmlNode& node, Components::SkyBoxComponent& component)
{
    component.SetDayTexture(ReadStringVector(node, CSTR_DAY_TEXTURES));
    component.SetNightTexture(ReadStringVector(node, CSTR_NIGHT_TEXTURES));
    component.SetModel(node.GetChild(CSTR_MODEL_FILE_NAME)->value_);
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
void AddComponent(Utils::XmlNode& node, GameObject& gameObject, const std::string& cstr)
{
    auto& comp = gameObject.AddComponent<T>();
    Read(*node.GetChild(cstr), comp);
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
                AddComponent<Components::Animator>(*component, gameObject, CSTR_COMPONENT_ANIMATOR);
                break;
            case Components::ComponentsType::BoxShape:
                AddComponent<Components::BoxShape>(*component, gameObject, CSTR_COMPONENT_BOXSHAPE);
                break;
            case Components::ComponentsType::MeshShape:
                AddComponent<Components::MeshShape>(*component, gameObject, CSTR_COMPONENT_MESHSHAPE);
                break;
            case Components::ComponentsType::SphereShape:
                AddComponent<Components::SphereShape>(*component, gameObject, CSTR_COMPONENT_SPHERESHAPE);
                break;
            case Components::ComponentsType::TerrainShape:
                AddComponent<Components::TerrainShape>(*component, gameObject, CSTR_COMPONENT_TERRAINSHAPE);
                break;
            case Components::ComponentsType::CollisionShape:
                break;
            case Components::ComponentsType::Rigidbody:
                AddComponent<Components::Rigidbody>(*component, gameObject, CSTR_COMPONENT_RIGIDBODY);
                break;
            case Components::ComponentsType::Renderer:
                AddComponent<Components::RendererComponent>(*component, gameObject, CSTR_COMPONENT_RENDERER);
                break;
            case Components::ComponentsType::TreeRenderer:
                AddComponent<Components::TreeRendererComponent>(*component, gameObject, CSTR_COMPONENT_TREERENDERER);
                break;
            case Components::ComponentsType::ParticleEffect:
                AddComponent<Components::ParticleEffectComponent>(*component, gameObject,
                                                                  CSTR_COMPONENT_PARTICLEEFFECT);
                break;
            case Components::ComponentsType::SkyBox:
                AddComponent<Components::SkyBoxComponent>(*component, gameObject, CSTR_COMPONENT_SKYBOX);
                break;
            case Components::ComponentsType::Skydome:
                //AddComponent<Components::SkydomeComponent>(*component, gameObject, CSTR_COMPONENT_SKYBOX);
                break;
            case Components::ComponentsType::Grass:
                AddComponent<Components::GrassRendererComponent>(*component, gameObject, CSTR_COMPONENT_GRASS);
                break;
            case Components::ComponentsType::TerrainRenderer:
                AddComponent<Components::TerrainRendererComponent>(*component, gameObject,
                                                                   CSTR_COMPONENT_TERRAINRENDERER);
                break;
            case Components::ComponentsType::TerrainMeshRenderer:
                AddComponent<Components::TerrainMeshRendererComponent>(*component, gameObject,
                                                                       CSTR_COMPONENT_TERRAINMESHRENDERER);
                break;
        }
    }
}

void Read(Utils::XmlNode& node, Scene& scene)
{
    for (const auto& child : node.GetChild(CSTR_GAMEOBJECTS)->GetChildren())
    {
        auto gameObject = scene.CreateGameObject(child->attributes_.at(CSTR_NAME));
        Read(*child, *gameObject);
        scene.AddGameObject(gameObject);
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
}  // namespace GameEngine
