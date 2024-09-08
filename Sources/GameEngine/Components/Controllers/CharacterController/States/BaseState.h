#pragma once
#include "CharacterControllerCommonDefs.h"

namespace GameEngine::Components
{
class BaseState
{
public:
    BaseState(FsmContext&);
    virtual ~BaseState() = default;
    
    void update(const TriggerJumpEvent&);

protected:
    FsmContext& context_;
};
}  // namespace GameEngine::Components
