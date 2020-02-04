#pragma once
#include <string>

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
} // namespace std
