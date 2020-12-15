#include "SceneWriter.h"

#include <Utils.h>
#include <Utils/GLM/GLMUtils.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

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
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/ResourceUtils.h"
#include "GameEngine/Scene/Scene.hpp"
#include "SceneDef.h"
#include "Utils/Variant.h"

using namespace Utils;

namespace GameEngine
{
void Create(XmlNode& node, float v)
{
    node.value_ = std::to_string(v);
}
void Create(XmlNode& node, int v)
{
    node.value_ = std::to_string(v);
}
void Create(XmlNode& node, uint32 v)
{
    node.value_ = std::to_string(v);
}
void Create(XmlNode& node, bool b)
{
    node.value_ = Utils::BoolToString(b);
}
void Create(XmlNode& node, const File& file)
{
    node.value_ = file.GetDataRelativeDir();
}
void Create(XmlNode& node, const std::string& str)
{
    node.value_ = str;
}
void Create(XmlNode& node, const vec2ui& v)
{
    node.attributes_[CSTR_X] = std::to_string(v.x);
    node.attributes_[CSTR_Y] = std::to_string(v.y);
}
void Create(XmlNode& node, const vec3& v)
{
    node.attributes_[CSTR_X] = std::to_string(v.x);
    node.attributes_[CSTR_Y] = std::to_string(v.y);
    node.attributes_[CSTR_Z] = std::to_string(v.z);
}
void Create(XmlNode& node, const vec4& v)
{
    node.attributes_[CSTR_X] = std::to_string(v.x);
    node.attributes_[CSTR_Y] = std::to_string(v.y);
    node.attributes_[CSTR_Z] = std::to_string(v.z);
    node.attributes_[CSTR_W] = std::to_string(v.w);
}

void Create(XmlNode& node, const Texture& texture)
{
    Create(node, *texture.GetFile());
}

void Create(XmlNode& node, const std::vector<vec3>& v)
{
    for (const auto& value : v)
    {
        Create(node.AddChild(CSTR_VEC3), value);
    }
}

void Create(XmlNode& node, const std::vector<std::string>& str)
{
    for (const auto& value : str)
    {
        Create(node.AddChild(CSTR_MODEL_FILE_NAME), value);
    }
}

void Create(XmlNode& node, const std::array<File, 6>& str)
{
    for (const auto& value : str)
    {
        Create(node.AddChild(CSTR_TEXTURE_FILENAME), value);
    }
}

void Create(XmlNode& node, const std::vector<float>& str)
{
    for (const auto& value : str)
    {
        Create(node.AddChild(CSTR_V), value);
    }
}

void Create(XmlNode&, const Components::ThridPersonCameraComponent&)
{
}

void Create(XmlNode& node, TerrainTextureType type, float scale, const std::string& filename)
{
    Create(node.AddChild(CSTR_TEXTURE_TYPE), std::to_string(type));
    Create(node.AddChild(CSTR_SCALE), std::to_string(scale));
    Create(node.AddChild(CSTR_TEXTURE_FILENAME), filename);
}

void Create(XmlNode& node, const std::vector<Components::TerrainComponentBase::TerrainTexture>& textures)
{
    for (const auto& value : textures)
    {
        Create(node.AddChild(CSTR_TEXTURE), value.type, value.tiledScale, value.file.GetDataRelativeDir());
    }
}

void Create(XmlNode& node, const common::Transform& transform)
{
    Create(node.AddChild(CSTR_POSITION), transform.GetPosition());
    Create(node.AddChild(CSTR_ROTATION), transform.GetRotation().GetEulerDegrees().value);
    Create(node.AddChild(CSTR_SCALE), transform.GetScale());
}

void Create(XmlNode& node, const std::string& filename, LevelOfDetail lvl)
{
    node.AddChild(CSTR_FILE_NAME).value_           = filename;
    node.AddChild(CSTR_MODEL_LVL_OF_DETAIL).value_ = std::to_string(static_cast<int>(lvl));
}

void Create(XmlNode& node, const Components::Animator& component)
{
    Create(node.AddChild(CSTR_CURRENT_ANIMATION), component.GetCurrentAnimationName());

    auto& animationClipsNode = node.AddChild(CSTR_ANIMATION_CLIPS);

    for (const auto& clip : component.animationClips_)
    {
        if (not clip.second.filePath.empty())
        {
            Create(animationClipsNode.AddChild(CSTR_ANIMATION_CLIP), clip.second.filePath);
        }
    }
}

void Create(XmlNode& node, const Components::BoxShape& component)
{
    Create(node.AddChild(CSTR_SIZE), component.GetSize());
    Create(node.AddChild(CSTR_POSITION_OFFSET), component.GetPositionOffset());
}

void Create(XmlNode& node, const Components::MeshShape& component)
{
    Create(node.AddChild(CSTR_SIZE), component.GetSize());
    Create(node.AddChild(CSTR_POSITION_OFFSET), component.GetPositionOffset());

    auto model = component.GetModel();
    Create(node.AddChild(CSTR_MODEL_FILE_NAME), model ? model->GetFile() : "");
}

void Create(XmlNode& node, const Components::SphereShape& component)
{
    Create(node.AddChild(CSTR_SIZE), component.GetSize());
    Create(node.AddChild(CSTR_POSITION_OFFSET), component.GetPositionOffset());
}

void Create(XmlNode& node, const Components::TerrainShape& component)
{
    Create(node.AddChild(CSTR_POSITION_OFFSET), component.GetPositionOffset());
    Create(node.AddChild(CSTR_HEIGHTMAP_FILENAME), component.GetHeightMapFile());
}

void Create(XmlNode& node, const Components::CapsuleShape& component)
{
    Create(node.AddChild(CSTR_POSITION_OFFSET), component.GetPositionOffset());
    Create(node.AddChild(CSTR_HEIGHT), component.GetHeight());
    Create(node.AddChild(CSTR_RADIUS), component.GetRadius());
}

void Create(XmlNode& node, const Components::Rigidbody& component)
{
    Create(node.AddChild(CSTR_MASS), component.GetMass());
    Create(node.AddChild(CSTR_IS_STATIC), component.IsStatic());
    Create(node.AddChild(CSTR_COLLISION_SHAPE), static_cast<int>(component.GetCollisionShapeType()));

    auto angularFactor = component.InputParams().angularFactor_;
    if (angularFactor)
    {
        Create(node.AddChild(CSTR_ANGULAR_FACTOR), *angularFactor);
    }

    auto velocty = component.InputParams().velocity_;
    if (velocty)
    {
        Create(node.AddChild(CSTR_VELOCITY), *velocty);
    }
}

void Create(XmlNode& node, const std::unordered_map<std::string, LevelOfDetail>& files)
{
    for (const auto& files : files)
    {
        Create(node.AddChild(CSTR_MODEL_FILE_NAME), files.first, files.second);
    }
}

void Create(XmlNode& node, const Components::RendererComponent& component)
{
    Create(node.AddChild(CSTR_MODEL_FILE_NAMES), component.GetFileNames());
    node.AddChild(CSTR_TEXTURE_INDEX).value_ = std::to_string(component.GetTextureIndex());
}

void Create(XmlNode& node, const Components::TreeRendererComponent& component)
{
    Create(node.AddChild(CSTR_TOP_FILENAMES), component.GetTopFileNames());
    Create(node.AddChild(CSTR_BOTTOM_FILENAMES), component.GetBottomFileNames());
    Create(node.AddChild(CSTR_SIZE_2D), component.GetPositionSize2d());
    Create(node.AddChild(CSTR_POSITIONS), component.GetCPositions());
}

void Create(XmlNode& node, const Particle& particle)
{
    Create(node.AddChild(CSTR_POSITION), particle.position);
    Create(node.AddChild(CSTR_ROTATION), particle.rotation);
    Create(node.AddChild(CSTR_SCALE), particle.scale);
    Create(node.AddChild(CSTR_VELOCITY), particle.velocity);
    Create(node.AddChild(CSTR_GRAVITY_EFFECT), particle.gravityEffect);
    Create(node.AddChild(CSTR_LIFE_TIME), particle.lifeTime);
}

void Create(XmlNode& node, const Components::ParticleEffectComponent& component)
{
    Create(node.AddChild(CSTR_PARTICLE), component.GetReferenceParticle());
    Create(node.AddChild(CSTR_TEXTURE), component.GetTextureFile());
    Create(node.AddChild(CSTR_PARTICLE_PER_SER), component.GetParticlePerSec());
    Create(node.AddChild(CSTR_BLEND_TYPE), static_cast<int>(component.GetBlendType()));
    Create(node.AddChild(CSTR_EMIT_FUNCTION), component.GetEmitFunction());
    Create(node.AddChild(CSTR_SPEED), component.GetParticleSpeed());
    Create(node.AddChild(CSTR_IS_ANIMATED), component.IsAnimated());
    Create(node.AddChild(CSTR_PARTICLE_LIMT), component.GetParticleLimit());
}

void Create(XmlNode& node, const Components::SkyBoxComponent& component)
{
    Create(node.AddChild(CSTR_DAY_TEXTURES), component.GetDayTextureFiles());
    Create(node.AddChild(CSTR_NIGHT_TEXTURES), component.GetNightTextureFiles());
    Create(node.AddChild(CSTR_MODEL_FILE_NAME), component.GetModelFileName());
}

void Create(XmlNode&, const Components::SkydomeComponent&)
{
}

void Create(XmlNode& node, const Components::PlayerInputController& component)
{
    auto& animationClipsNode = node.AddChild(CSTR_ANIMATION_CLIPS);

    node.AddChild(CSTR_IDLE_ANIMATION).value_ = component.idleAnimationName_;
    node.AddChild(CSTR_WALK_ANIMATION).value_ = component.walkAnimationName_;
    node.AddChild(CSTR_RUN_ANIMATION).value_  = component.runAnimationName_;
}

void Create(XmlNode&, const Components::CharacterController&)
{
}

void Create(XmlNode& node, const Components::WaterRendererComponent& component)
{
    Create(node.AddChild(CSTR_COLOR), component.GetWaterColor());
    Create(node.AddChild(CSTR_WAVE_SPEED), component.GetWaveSpeed());

    if (component.GetDudvTexture())
        Create(node.AddChild(CSTR_DUDV_MAP), *component.GetDudvTexture());

    if (component.GetNormalTexture())
        Create(node.AddChild(CSTR_NORMAL_MAP), *component.GetNormalTexture());
}

std::string DataVectorToString(const std::vector<float>& input)
{
    std::string result;
    for (const auto& p : input)
    {
        result += std::to_string(p) + ' ';
    }
    result.pop_back();
    return result;
}

void Create(XmlNode& node, const Components::GrassRendererComponent& component)
{
    const auto& meshData = component.GetGrassMeshesData();

    auto file = component.getDataFile();

    if (file.empty())
        file.DataRelative("Generated/grassMeshData_" + std::to_string(component.getParentGameObject().GetId()) +
                          ".bin");

    auto opened = file.openToWrite();
    if (opened)
    {
        file.addVectorToFile(meshData.positions);
        file.addVectorToFile(meshData.normals);
        file.addVectorToFile(meshData.colors);
        file.addVectorToFile(meshData.sizesAndRotations);
        file.close();
    }

    Create(node.AddChild(CSTR_FILE_NAME), file);
    Create(node.AddChild(CSTR_TEXTURE_FILENAME), component.getTextureFile());
}

void Create(XmlNode& node, const Components::TerrainRendererComponent& component)
{
    Create(node.AddChild(CSTR_TEXTURE_FILENAMES), component.GetInputDataTextures());

    auto heightMapTexture = component.GetTexture(TerrainTextureType::heightmap);

    if (heightMapTexture and heightMapTexture->IsModified())
    {
        auto heightMap = static_cast<HeightMap*>(heightMapTexture);
        if (heightMap and heightMap->GetFile())
        {
            Utils::CreateBackupFile(heightMap->GetFile()->GetAbsoultePath());
            SaveHeightMap(*heightMap, heightMap->GetFile()->GetAbsoultePath());
        }
        else
        {
            ERROR_LOG("Heightmap texture cast error");
        }
    }

    auto blendMapTexture = component.GetTexture(TerrainTextureType::blendMap);

    if (blendMapTexture)
    {
        if (blendMapTexture->IsModified() and blendMapTexture->GetFile())
        {
            auto blendMap     = static_cast<GeneralTexture*>(blendMapTexture);
            const auto& image = blendMap->GetImage();
            Utils::CreateBackupFile(blendMapTexture->GetFile()->GetAbsoultePath());

            std::visit(visitor{
                           [&](const std::vector<uint8>& data) {
                               Utils::SaveImage(data, image.size(), blendMapTexture->GetFile()->GetAbsoultePath());
                           },
                           [](const std::vector<float>& data) { DEBUG_LOG("Float version not implemented."); },
                           [](const std::monostate&) { ERROR_LOG("Image data is not set!"); },
                       },
                       image.getImageData());
        }
    }
}

template <typename T>
void CreateComponent(XmlNode& node, const Components::IComponent& component)
{
    auto comp = static_cast<const T*>(&component);
    Create(node, *comp);
}

void Create(XmlNode& node, const Components::IComponent& component)
{
    node.attributes_[CSTR_TYPE] = std::to_string(component.GetType());

    switch (component.GetType())
    {
        case Components::ComponentsType::Animator:
            CreateComponent<Components::Animator>(node, component);
            break;
        case Components::ComponentsType::BoxShape:
            CreateComponent<Components::BoxShape>(node, component);
            break;
        case Components::ComponentsType::MeshShape:
            CreateComponent<Components::MeshShape>(node, component);
            break;
        case Components::ComponentsType::SphereShape:
            CreateComponent<Components::SphereShape>(node, component);
            break;
        case Components::ComponentsType::TerrainShape:
            CreateComponent<Components::TerrainShape>(node, component);
            break;
        case Components::ComponentsType::CapsuleShape:
            CreateComponent<Components::CapsuleShape>(node, component);
            break;
        case Components::ComponentsType::Rigidbody:
            CreateComponent<Components::Rigidbody>(node, component);
            break;
        case Components::ComponentsType::Renderer:
            CreateComponent<Components::RendererComponent>(node, component);
            break;
        case Components::ComponentsType::TreeRenderer:
            CreateComponent<Components::TreeRendererComponent>(node, component);
            break;
        case Components::ComponentsType::ParticleEffect:
            CreateComponent<Components::ParticleEffectComponent>(node, component);
            break;
        case Components::ComponentsType::SkyBox:
            CreateComponent<Components::SkyBoxComponent>(node, component);
            break;
        case Components::ComponentsType::Skydome:
            CreateComponent<Components::SkydomeComponent>(node, component);
            break;
        case Components::ComponentsType::Grass:
            CreateComponent<Components::GrassRendererComponent>(node, component);
            break;
        case Components::ComponentsType::TerrainRenderer:
            CreateComponent<Components::TerrainRendererComponent>(node, component);
            break;
        case Components::ComponentsType::Water:
            CreateComponent<Components::WaterRendererComponent>(node, component);
            break;
        case Components::ComponentsType::ThridPersonCamera:
            CreateComponent<Components::ThridPersonCameraComponent>(node, component);
            break;
        case Components::ComponentsType::CharacterController:
            CreateComponent<Components::CharacterController>(node, component);
            break;
        case Components::ComponentsType::PlayerInputController:
            CreateComponent<Components::PlayerInputController>(node, component);
            break;
    }
}

void Create(XmlNode& node, const std::vector<std::unique_ptr<Components::IComponent>>& components)
{
    node.attributes_[CSTR_COUNT] = std::to_string(components.size());
    for (const auto& component : components)
    {
        Create(node.AddChild(CSTR_COMPONENT), *component);
    }
}

void Create(XmlNode& node, const GameObject& gameObject)
{
    node.attributes_.insert({CSTR_NAME, gameObject.GetName()});
    node.attributes_.insert({CSTR_ID, std::to_string(gameObject.GetId())});
    Create(node.AddChild(CSTR_TRANSFORM), gameObject.GetTransform());
    Create(node.AddChild(CSTR_COMPONENTS), gameObject.GetComponents());

    if (not gameObject.GetChildren().empty())
    {
        auto& childrenNode = node.AddChild(CSTR_CHILDREN);

        for (const auto& gameObject : gameObject.GetChildren())
        {
            Create(childrenNode.AddChild(CSTR_GAMEOBJECT), *gameObject);
        }
    }
}

void Create(XmlNode& node, const GameObjects& gameObjects)
{
    node.attributes_[CSTR_COUNT] = std::to_string(gameObjects.size());

    for (const auto& gameObject : gameObjects)
    {
        Create(node.AddChild(CSTR_GAMEOBJECT), *gameObject);
    }
}
Utils::XmlNode createTree(const Scene& input)
{
    XmlNode scene(CSTR_SCENE);
    scene.attributes_[CSTR_NAME] = input.GetName();
    Create(scene.AddChild(CSTR_GAMEOBJECTS), input.GetGameObjects());
    return scene;
}
Utils::XmlNode createPrefab(const GameObject& gameObject)
{
    Utils::XmlNode rootNode(CSTR_PREFAB);
    Create(rootNode, gameObject);
    return rootNode;
}
}  // namespace GameEngine
