#pragma once

namespace Utils
{
namespace StateMachine
{
template <typename Event, typename Action>
struct On
{
    Action handle(const Event&) const
    {
        return Action{};
    }
};
}  // namespace StateMachine
}  // namespace Utils
