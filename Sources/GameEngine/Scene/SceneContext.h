#pragma once

namespace GraphicsApi
{
class IGraphicsApi;
}  // namespace GraphicsApi
namespace Input
{
class InputManager;
}  // namespace Input

namespace GameEngine
{
class DisplayManager;
namespace Renderer
{
class RenderersManager;
}  // namespace Renderer
namespace Physics
{
class IPhysicsApi;
}  // namespace Physics

struct SceneContext
{
    GraphicsApi::IGraphicsApi& graphicsApi_;
    Input::InputManager& inputManager;
    DisplayManager& displayManager;
    Renderer::RenderersManager& renderersManager;
    Physics::IPhysicsApi& physicsApi;
};
}  // namespace GameEngine
