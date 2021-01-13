#include "ComponentsTypes.h"
#include <Logger/Log.h>

#define RETURN_STR(x)       \
    case ComponentsType::x: \
        return (#x)
#define FROM_STR(x) \
    if (name == #x) \
    return ComponentsType::x

using namespace GameEngine::Components;

namespace GameEngine
{
namespace Components
{
std::vector<ComponentsType> GetComponentTypes()
{
    // clang-format off
    return
    {
        ComponentsType::Animator,
        ComponentsType::BoxShape,
        ComponentsType::MeshShape,
        ComponentsType::SphereShape,
        ComponentsType::TerrainShape,
        ComponentsType::CapsuleShape,
        ComponentsType::Rigidbody,
        ComponentsType::Renderer,
        ComponentsType::PreviewComponent,
        ComponentsType::TreeRenderer,
        ComponentsType::ParticleEffect,
        ComponentsType::SkyBox,
        ComponentsType::Skydome,
        ComponentsType::Grass,
        ComponentsType::TerrainRenderer,
        ComponentsType::Water,
        ComponentsType::ThridPersonCamera,
        ComponentsType::PlayerInputController,
        ComponentsType::CharacterController,
        ComponentsType::EnemyController,
        ComponentsType::Player,
        ComponentsType::Enemy
    };
    // clang-format on
}
std::optional<GameEngine::Components::ComponentsType> from_string(const std::string& name)
{
    FROM_STR(Animator);
    FROM_STR(BoxShape);
    FROM_STR(MeshShape);
    FROM_STR(SphereShape);
    FROM_STR(TerrainShape);
    FROM_STR(CapsuleShape);
    FROM_STR(Rigidbody);
    FROM_STR(PreviewComponent);
    FROM_STR(Renderer);
    FROM_STR(TreeRenderer);
    FROM_STR(ParticleEffect);
    FROM_STR(SkyBox);
    FROM_STR(Skydome);
    FROM_STR(Grass);
    FROM_STR(TerrainRenderer);
    FROM_STR(Water);
    FROM_STR(ThridPersonCamera);
    FROM_STR(PlayerInputController);
    FROM_STR(CharacterController);
    FROM_STR(EnemyController);
    FROM_STR(Player);
    FROM_STR(Enemy);
    ERROR_LOG("Invalid name : " + name);
    return {};
}
}  // namespace Components
}  // namespace GameEngine

namespace std
{
std::string to_string(GameEngine::Components::ComponentsType type)
{
    switch (type)
    {
        RETURN_STR(Animator);
        RETURN_STR(BoxShape);
        RETURN_STR(MeshShape);
        RETURN_STR(SphereShape);
        RETURN_STR(TerrainShape);
        RETURN_STR(CapsuleShape);
        RETURN_STR(Rigidbody);
        RETURN_STR(Renderer);
        RETURN_STR(PreviewComponent);
        RETURN_STR(TreeRenderer);
        RETURN_STR(ParticleEffect);
        RETURN_STR(SkyBox);
        RETURN_STR(Skydome);
        RETURN_STR(Grass);
        RETURN_STR(TerrainRenderer);
        RETURN_STR(Water);
        RETURN_STR(ThridPersonCamera);
        RETURN_STR(PlayerInputController);
        RETURN_STR(CharacterController);
        RETURN_STR(EnemyController);
        RETURN_STR(Player);
        RETURN_STR(Enemy);
    }
    return std::to_string(static_cast<int>(type));
}
}  // namespace std
#undef RETURN_STR
#undef FROM_STR
