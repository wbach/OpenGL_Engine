#pragma once

namespace GameEngine
{
namespace Components
{
enum class FunctionType
{
    Awake,
    OnStart,
    PostStart,
    Update,
    PostUpdate,
    AlwaysUpdate,
};
}
}  // namespace GameEngine
