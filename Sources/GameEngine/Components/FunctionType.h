#pragma once

namespace GameEngine
{
namespace Components
{
enum class FunctionType
{
    Awake,
    OnStart,
    Update,
    PostUpdate,
    AlwaysUpdate,
};
}
}  // namespace GameEngine
