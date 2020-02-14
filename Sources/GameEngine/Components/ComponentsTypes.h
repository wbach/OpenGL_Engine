#pragma once
#include <string>
#include <optional>

namespace GameEngine
{
namespace Components
{
enum class ComponentsType
{
    Animator,
    BoxShape,
    MeshShape,
    SphereShape,
    TerrainShape,
    CapsuleShape,
    Rigidbody,
    Renderer,
    TreeRenderer,
    ParticleEffect,
    SkyBox,
    Skydome,
    Grass,
    TerrainRenderer,
    TerrainMeshRenderer,
    Water,
    ThridPersonCamera,
    PlayerInputController,
    CharacterController,
};
}  // namespace Components
}  // namespace GameEngine

namespace std
{
std::string to_string(GameEngine::Components::ComponentsType);
std::optional<GameEngine::Components::ComponentsType> from_string(const std::string&);
} // namespace std
