#pragma once
#include <Types.h>

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

    Task(IdType id, Action action)
        : action{action}
        , id{id}
    {
    }
    void execute()
    {
        if (action)
            action();
    }

    IdType GetId() const
    {
        return id;
    }

private:
    Action action;
    IdType id;
};
}  // namespace Bullet
}  // namespace Physics
}  // namespace GameEngine
