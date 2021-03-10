#pragma once

#include "OneOf.h"
#include "Nothing.h"

namespace Utils
{
namespace StateMachine
{
template <typename Action>
struct Maybe : public OneOf<Action, Nothing>
{
    using OneOf<Action, Nothing>::OneOf;
};
}  // namespace StateMachine
}  // namespace Utils

