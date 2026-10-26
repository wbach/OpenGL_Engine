#pragma once
#include "BaseState.h"
#include "CharacterControllerCommonDefs.h"
#include "StateConcepts.h"

namespace GameEngine
{
namespace Components
{
class DeathState : public BaseState,
                   public Utils::StateMachine::Will<Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>>
{
public:
    DeathState(FsmContext&);

    void onEnter();

    template <typename PrevState, typename Event>
        requires IsCrouchArmedChange<PrevState>
    void onEnter(PrevState&, const Event&)
    {
        setArmedChangeInCrouchRelatedClip();
    }

    template <typename PrevState, typename Event>
        requires IsArmedChange<PrevState>
    void onEnter(PrevState&, const Event&)
    {
        setArmedChangeRelatedClip();
    }

    template <typename PrevState, typename Event>
        requires IsCrouchDisarmed<PrevState>
    void onEnter(PrevState&, const Event&)
    {
        setCrouchDisarmedRelatedClip();
    }

    template <typename PrevState, typename Event>
        requires IsCrouchArmed<PrevState>
    void onEnter(PrevState&, const Event&)
    {
        setCrouchArmedRelatedClip();
    }

    template <typename PrevState, typename Event>
        requires IsDisarmed<PrevState>
    void onEnter(PrevState&, const Event&)
    {
        setDisarmedRelatedClip();
    }

    template <typename PrevState, typename Event>
        requires IsArmed<PrevState>
    void onEnter(PrevState&, const Event&)
    {
        setArmedRelatedClip();
    }

private:
    void setAnim(const std::string&);
    void setArmedRelatedClip();
    void setDisarmedRelatedClip();
    void setCrouchArmedRelatedClip();
    void setCrouchDisarmedRelatedClip();
    void setArmedChangeRelatedClip();
    void setArmedChangeInCrouchRelatedClip();
};
}  // namespace Components
}  // namespace GameEngine
