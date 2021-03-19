#pragma once
#include <string>
#include <optional>

namespace GameEngine
{
namespace Components
{
struct StopAnimationEvent
{
    std::optional<std::string> jointGroupName;
};
}  // namespace Components
}  // namespace GameEngine
