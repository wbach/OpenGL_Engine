#pragma once

namespace GraphicsApi
{
class IGraphicsApi;
}  // namespace GraphicsApi
namespace Input
{
class InputManager;
}  // namespace Input
namespace Physics
{
class IPhysicsApi;
}  // namespace Physics

namespace GameEngine
{
class DisplayManager;
namespace Renderer
{
class RenderersManager;
}  // namespace Renderer

struct SceneInitContext
{
    GraphicsApi::IGraphicsApi* graphicsApi_;
    Input::InputManager* inputManager;
    DisplayManager* displayManager;
    Renderer::RenderersManager* renderersManager;
    Physics::IPhysicsApi* physicsApi;
};
}  // namespace GameEngine
