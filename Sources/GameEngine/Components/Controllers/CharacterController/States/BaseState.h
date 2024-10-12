#pragma once
#include "CharacterControllerCommonDefs.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine::Components
{
class BaseState
{
public:
    BaseState(FsmContext&);
    virtual ~BaseState() = default;

    void update(float);

protected:
    FsmContext& context_;
};
}  // namespace GameEngine::Components
