#pragma once
namespace Utils
{
namespace StateMachine
{
struct Nothing
{
    template <typename Machine, typename State, typename Event>
    void execute(Machine&, State&, const Event&)
    {
    }
};
}  // namespace StateMachine
}  // namespace Utils

