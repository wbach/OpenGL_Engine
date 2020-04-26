#include "ComponentFactory.h"
#include "Animation/Animator.h"
#include "BaseComponent.h"
#include "Camera/ThridPersonCameraComponent.h"
#include "Controllers/CharacterController.h"
#include "Input/PlayerInputController.h"
#include "Physics/BoxShape.h"
#include "Physics/CapsuleShape.h"
#include "Physics/MeshShape.h"
#include "Physics/Rigidbody.h"
#include "Physics/SphereShape.h"
#include "Physics/Terrain/TerrainShape.h"
#include "Renderer/Entity/RendererComponent.hpp"
#include "Renderer/Grass/GrassComponent.h"
#include "Renderer/Particles/ParticleEffectComponent.h"
#include "Renderer/SkyBox/SkyBoxComponent.h"
#include "Renderer/Skydome/SkydomeComponent.h"
#include "Renderer/Terrain/TerrainMeshRendererComponent.h"
#include "Renderer/Terrain/TerrainRendererComponent.h"
#include "Renderer/Trees/TreeRendererComponent.h"
#include "Renderer/Water/WaterRendererComponent.h"

namespace GameEngine
{
namespace Components
{
ComponentFactory::ComponentFactory(ComponentController& componentController, GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& gpuResourceLoader, Time& time, Input::InputManager& input,
                                   IResourceManager& resourceManager, Renderer::RenderersManager& rendererManager,
                                   CameraWrapper& camera, Physics::IPhysicsApi& physicsApi)
    : context_(graphicsApi, gpuResourceLoader, time, input, camera, physicsApi, resourceManager, rendererManager, componentController)
{
}
std::unique_ptr<IComponent> ComponentFactory::Create(ComponentsType type, GameObject& ptr)
{
    switch (type)
    {
        case ComponentsType::Animator:
        {
            return std::make_unique<Animator>(context_, ptr);
        }
        case ComponentsType::Renderer:
        {
            return std::make_unique<RendererComponent>(context_, ptr);
        }
        case ComponentsType::TreeRenderer:
        {
            return std::make_unique<TreeRendererComponent>(context_, ptr);
        }
        case ComponentsType::ParticleEffect:
        {
            return std::make_unique<ParticleEffectComponent>(context_, ptr);
        }
        case ComponentsType::BoxShape:
        {
            return std::make_unique<BoxShape>(context_, ptr);
        }
        case ComponentsType::SphereShape:
        {
            return std::make_unique<SphereShape>(context_, ptr);
        }
        case ComponentsType::CapsuleShape:
        {
            return std::make_unique<CapsuleShape>(context_, ptr);
        }
        case ComponentsType::TerrainShape:
        {
            return std::make_unique<TerrainShape>(context_, ptr);
        }
        case ComponentsType::MeshShape:
        {
            return std::make_unique<MeshShape>(context_, ptr);
        }
        case ComponentsType::Rigidbody:
        {
            return std::make_unique<Rigidbody>(context_, ptr);
        }
        case ComponentsType::SkyBox:
        {
            return std::make_unique<SkyBoxComponent>(context_, ptr);
        }
        case ComponentsType::Skydome:
        {
            return std::make_unique<SkydomeComponent>(context_, ptr);
        }
        case ComponentsType::TerrainRenderer:
        {
            return std::make_unique<TerrainRendererComponent>(context_, ptr);
        }
        case ComponentsType::Grass:
        {
            return std::make_unique<GrassRendererComponent>(context_, ptr);
        }
        case ComponentsType::Water:
        {
            return std::make_unique<WaterRendererComponent>(context_, ptr);
        }
        case ComponentsType::ThridPersonCamera:
        {
            return std::make_unique<ThridPersonCameraComponent>(context_, ptr);
        }
        case ComponentsType::CharacterController:
        {
            return std::make_unique<CharacterController>(context_, ptr);
        }
        case ComponentsType::PlayerInputController:
        {
            return std::make_unique<PlayerInputController>(context_, ptr);
        }
    }
    return nullptr;
}
}  // namespace Components
}  // namespace GameEngine
