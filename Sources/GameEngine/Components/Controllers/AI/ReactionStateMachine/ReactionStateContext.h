#pragma once

namespace GameEngine
{
class GameObject;
namespace Components
{
class AIController;
struct ReactionStateContext
{
    GameObject& gameObject;
    AIController& controller;
};
}  // namespace Components
}  // namespace GameEngine
