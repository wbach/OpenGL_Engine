#pragma once
namespace Utils
{
namespace StateMachine
{
template <typename... Handlers>
struct Will : Handlers...
{
    using Handlers::handle...;
};
}  // namespace StateMachine
}  // namespace Utils
