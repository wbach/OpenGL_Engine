#pragma once

namespace GameEngine
{
namespace Components
{
enum class FunctionType
{
    Awake,
    LateAwake,
    OnStart,
    PostStart,
    Update,
    PostUpdate,
    AlwaysUpdate,
};
}
}  // namespace GameEngine
