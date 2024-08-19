#pragma once
#include <functional>
#include <optional>
#include <string>

#include "../CharacterControllerEvents.h"

namespace GameEngine
{
namespace Components
{
struct FsmContext;
struct JumpStateBase
{
public:
    JumpStateBase(FsmContext&, const std::optional<std::string>&, std::function<void()>);
    void onEnter(const JumpEvent&);
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
