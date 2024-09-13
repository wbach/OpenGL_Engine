#pragma once
#include "CharacterControllerCommonDefs.h"

namespace GameEngine::Components
{
extern float DEFAULT_FALLING_DETECT_THRESHOLD;

class BaseState
{
public:
    BaseState(FsmContext&, float =DEFAULT_FALLING_DETECT_THRESHOLD);
    virtual ~BaseState() = default;
    
    void update(const TriggerJumpEvent&);
    void update(float);

protected:
    FsmContext& context_;
    float fallingDetectThreshold;
};
}  // namespace GameEngine::Components
