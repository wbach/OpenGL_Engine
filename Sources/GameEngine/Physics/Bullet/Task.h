#pragma once
#include <functional>

namespace GameEngine
{
namespace Physics
{
namespace Bullet
{
struct Task
{
public:
    using Action = std::function<void()>;

    Task(Action action)
        : action{action}
    {
    }
    void execute()
    {
        if (action)
            action();
    }

private:
    Action action;
};
}  // namespace Bullet
}  // namespace Physics
}  // namespace GameEngine
