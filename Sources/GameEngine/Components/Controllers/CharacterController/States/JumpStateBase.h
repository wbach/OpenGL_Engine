#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
struct JumpStateBase
{
public:
    JumpStateBase(FsmContext&, const std::optional<std::string>&, std::function<void()>);
    void onEnter(const JumpEvent&);
    void onEnter(const AttackEvent&);
    void onEnter(const EndAttackEvent&);
    void update(float);

private:
    bool isGrounded();

private:
    FsmContext& context_;
    std::optional<std::string> jointGroupName_;
    std::function<void()> endCallback_;
};
}  // namespace Components
}  // namespace GameEngine
