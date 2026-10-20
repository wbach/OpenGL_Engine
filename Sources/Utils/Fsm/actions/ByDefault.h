#pragma once
#include "Logger/Log.h"
#include "Utils.h"

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
        LOG_DEBUG << "No action is defined for event of type: " << Utils::GetTypeName<Event>();
        return Action{};
    }
};
}  // namespace StateMachine
}  // namespace Utils
