#include "../CharacterControllerTests.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Narrative/Dialogs/Fsm/DialogEvents.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.triggerAndExpect({test.sut_.animationClipsNames_.disarmed.posture.stand.idle}, {ADVANCED_TIME_TRANSITION_TIME},
                          StartDialogEvent{.role = StartDialogEvent::Role::Waiting});
}
}  // namespace

TEST_F(CharacterControllerTests, DialogState_StartDialog_Listen)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    triggerAndExpect({sut_.animationClipsNames_.disarmed.dialog.listen}, {ADVANCED_TIME_TRANSITION_TIME},
                     StartDialogEvent{.role = StartDialogEvent::Role::Listener});
}

TEST_F(CharacterControllerTests, DialogState_StartDialog_Speaker)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    triggerAndExpect({sut_.animationClipsNames_.disarmed.dialog.say}, {ADVANCED_TIME_TRANSITION_TIME},
                     StartDialogEvent{.role = StartDialogEvent::Role::Speaker});
}

TEST_F(CharacterControllerTests, DialogState_StartDialog_UpdateListenerToSpeaker)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    prepareState(*this);

    triggerAndExpect({sut_.animationClipsNames_.disarmed.dialog.say}, {ADVANCED_TIME_TRANSITION_TIME},
                     StartDialogEvent{.role = StartDialogEvent::Role::Speaker});
}

TEST_F(CharacterControllerTests, DialogState_EndDialogEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    prepareState(*this);

    triggerAndExpect({sut_.animationClipsNames_.disarmed.dialog.say}, {ADVANCED_TIME_TRANSITION_TIME},
                     StartDialogEvent{.role = StartDialogEvent::Role::Speaker});
    triggerAndExpect<EndDialogEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle}, {ADVANCED_TIME_TRANSITION_TIME});
}

TEST_F(CharacterControllerTests, DialogState_MoveEvent)
{
    prepareState(*this);
    expectIgnoreEvent<MoveEvent>();
}

TEST_F(CharacterControllerTests, DialogState_MoveForwardEvent)
{
    prepareState(*this);
    expectIgnoreEvent<MoveForwardEvent>();
}

TEST_F(CharacterControllerTests, DialogState_MoveBackwardEvent)
{
    prepareState(*this);
    expectIgnoreEvent<MoveBackwardEvent>();
}

TEST_F(CharacterControllerTests, DialogState_MoveLeftEvent)
{
    prepareState(*this);
    expectIgnoreEvent<MoveLeftEvent>();
}

TEST_F(CharacterControllerTests, DialogState_MoveRightEvent)
{
    prepareState(*this);
    expectIgnoreEvent<MoveRightEvent>();
}

TEST_F(CharacterControllerTests, DialogState_WalkChangeStateEvent)
{
    prepareState(*this);
    expectIgnoreEvent<WalkChangeStateEvent>();
}

TEST_F(CharacterControllerTests, DialogState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectIgnoreEvent<SprintStateChangeEvent>();
}

TEST_F(CharacterControllerTests, DialogState_EndForwardMoveEvent)
{
    prepareState(*this);
    expectIgnoreEvent<EndForwardMoveEvent>();
}

TEST_F(CharacterControllerTests, DialogState_EndBackwardMoveEvent)
{
    prepareState(*this);
    expectIgnoreEvent<EndBackwardMoveEvent>();
}

TEST_F(CharacterControllerTests, DialogState_EndMoveLeftEvent)
{
    prepareState(*this);
    expectIgnoreEvent<EndMoveLeftEvent>();
}

TEST_F(CharacterControllerTests, DialogState_EndMoveRightEvent)
{
    prepareState(*this);
    expectIgnoreEvent<EndMoveRightEvent>();
}

TEST_F(CharacterControllerTests, DialogState_RotateLeftEvent)
{
    prepareState(*this);
    expectIgnoreEvent<RotateLeftEvent>();
}

TEST_F(CharacterControllerTests, DialogState_RotateRightEvent)
{
    prepareState(*this);
    expectIgnoreEvent<RotateRightEvent>();
}

TEST_F(CharacterControllerTests, DialogState_RotateTargetEvent)
{
    prepareState(*this);
    expectIgnoreEvent<RotateTargetEvent>();
}

TEST_F(CharacterControllerTests, DialogState_EndRotationEvent)
{
    prepareState(*this);
    expectIgnoreEvent<EndRotationEvent>();
}

TEST_F(CharacterControllerTests, DialogState_JumpEvent)
{
    prepareState(*this);
    expectIgnoreEvent<JumpEvent>();
}

TEST_F(CharacterControllerTests, DialogState_JumpConfirmEvent)
{
    prepareState(*this);
    expectIgnoreEvent<JumpConfirmEvent>();
}

TEST_F(CharacterControllerTests, DialogState_DodgeDiveEvent)
{
    prepareState(*this);
    expectIgnoreEvent<DodgeDiveEvent>();
}

TEST_F(CharacterControllerTests, DialogState_DodgeForwardEvent)
{
    prepareState(*this);
    expectIgnoreEvent<DodgeForwardEvent>();
}

TEST_F(CharacterControllerTests, DialogState_DodgeBackwardEvent)
{
    prepareState(*this);
    expectIgnoreEvent<DodgeBackwardEvent>();
}

TEST_F(CharacterControllerTests, DialogState_DodgeLeftEvent)
{
    prepareState(*this);
    expectIgnoreEvent<DodgeLeftEvent>();
}

TEST_F(CharacterControllerTests, DialogState_DodgeRightEvent)
{
    prepareState(*this);
    expectIgnoreEvent<DodgeRightEvent>();
}

TEST_F(CharacterControllerTests, DialogState_DodgeEndEvent)
{
    prepareState(*this);
    expectIgnoreEvent<DodgeEndEvent>();
}

TEST_F(CharacterControllerTests, DialogState_AttackEvent)
{
    prepareState(*this);
    expectIgnoreEvent<AttackEvent>();
}

TEST_F(CharacterControllerTests, DialogState_EndAttackEvent)
{
    prepareState(*this);
    expectIgnoreEvent<EndAttackEvent>();
}

TEST_F(CharacterControllerTests, DialogState_DeathEvent)
{
    prepareState(*this);
    triggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.posture.stand.death});
}

TEST_F(CharacterControllerTests, DialogState_WeaponStateEvent)
{
    prepareState(*this);
    expectIgnoreEvent<WeaponStateEvent>();
}

TEST_F(CharacterControllerTests, DialogState_EquipEndStateEvent)
{
    prepareState(*this);
    expectIgnoreEvent<EquipEndStateEvent>();
}

TEST_F(CharacterControllerTests, DialogState_DisarmEndStateEvent)
{
    prepareState(*this);
    expectIgnoreEvent<DisarmEndStateEvent>();
}

TEST_F(CharacterControllerTests, DialogState_DrawArrowEvent)
{
    prepareState(*this);
    expectIgnoreEvent<DrawArrowEvent>();
}

TEST_F(CharacterControllerTests, DialogState_ReloadArrowEvent)
{
    prepareState(*this);
    expectIgnoreEvent<ReloadArrowEvent>();
}

TEST_F(CharacterControllerTests, DialogState_AimStartEvent)
{
    prepareState(*this);
    expectIgnoreEvent<AimStartEvent>();
}

TEST_F(CharacterControllerTests, DialogState_AimStopEvent)
{
    prepareState(*this);
    expectIgnoreEvent<AimStopEvent>();
}

TEST_F(CharacterControllerTests, DialogState_CrouchChangeStateEvent)
{
    prepareState(*this);
    expectIgnoreEvent<CrouchChangeStateEvent>();
}

TEST_F(CharacterControllerTests, DialogState_StartFallingEvent)
{
    prepareState(*this);
    triggerAndExpect<StartFallingEvent>({sut_.animationClipsNames_.disarmed.falling});
}

TEST_F(CharacterControllerTests, DialogState_GroundDetectionEvent)
{
    prepareState(*this);
    expectIgnoreEvent<GroundDetectionEvent>();
}

TEST_F(CharacterControllerTests, DialogState_HurtEvent)
{
    prepareState(*this);
    triggerAndExpect<HurtEvent>({sut_.animationClipsNames_.disarmed.posture.stand.hurt});
}

TEST_F(CharacterControllerTests, DialogState_AnimationPlayEndEvent)
{
    prepareState(*this);
    expectIgnoreEvent<AnimationPlayEndEvent>();
}

TEST_F(CharacterControllerTests, DialogState_ChangeAnimEvent)
{
    prepareState(*this);
    expectIgnoreEvent<ChangeAnimEvent>();
}