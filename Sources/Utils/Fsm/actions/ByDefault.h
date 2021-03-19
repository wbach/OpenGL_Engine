#pragma once
namespace Utils
{
namespace StateMachine
{
template <typename Action>
struct ByDefault
{
    template <typename Event>
    Action handle(const Event&) const
    {
        return Action{};
    }
};
}  // namespace StateMachine
}  // namespace Utils
