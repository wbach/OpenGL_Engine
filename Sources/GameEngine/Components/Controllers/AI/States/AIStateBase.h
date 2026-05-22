#pragma once
#include <Utils/Fsm/Actions.h>

#include "../AIControllerEvents.h"

namespace GameEngine
{
namespace Components
{
struct AIControllerContext;

class AIStateBase
{
public:
    AIStateBase(AIControllerContext&);

    void onEnter();
    void onLeave();

protected:
    enum class NavigationStatus
    {
        InProgress,
        Done,
        Failed
    };

    NavigationStatus updateNavigation();
    void startMoveTo(const vec3&, AIMoveType);

protected:
    AIControllerContext& context_;
    AIMoveType moveType_;
    bool isMovingForward_{false};
};
}  // namespace Components
}  // namespace GameEngine
