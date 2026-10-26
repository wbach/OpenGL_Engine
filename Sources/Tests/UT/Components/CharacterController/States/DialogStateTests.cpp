#include "../CharacterControllerTests.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Narrative/Dialogs/Fsm/DialogEvents.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.tiggerAndExpect({test.sut_.animationClipsNames_.disarmed.posture.stand.idle}, {ADVANCED_TIME_TRANSITION_TIME},
                         StartDialogEvent{.role = StartDialogEvent::Role::Waiting});
}
}  // namespace

TEST_F(CharacterControllerTests, DialogState_StartDialogListen)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    tiggerAndExpect({sut_.animationClipsNames_.disarmed.dialog.listen}, {ADVANCED_TIME_TRANSITION_TIME},
                    StartDialogEvent{.role = StartDialogEvent::Role::Listener});
}

TEST_F(CharacterControllerTests, DialogState_StartDialogSay)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    tiggerAndExpect({sut_.animationClipsNames_.disarmed.dialog.say}, {ADVANCED_TIME_TRANSITION_TIME},
                    StartDialogEvent{.role = StartDialogEvent::Role::Speaker});
}

TEST_F(CharacterControllerTests, DialogState_StartDialogUpdateToSay)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    prepareState(*this);

    tiggerAndExpect({sut_.animationClipsNames_.disarmed.dialog.say}, {ADVANCED_TIME_TRANSITION_TIME},
                    StartDialogEvent{.role = StartDialogEvent::Role::Speaker});
}

TEST_F(CharacterControllerTests, DialogState_End)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    prepareState(*this);

    tiggerAndExpect({sut_.animationClipsNames_.disarmed.dialog.say}, {ADVANCED_TIME_TRANSITION_TIME},
                    StartDialogEvent{.role = StartDialogEvent::Role::Speaker});
    tiggerAndExpect({sut_.animationClipsNames_.disarmed.posture.stand.idle}, {ADVANCED_TIME_TRANSITION_TIME}, EndDialogEvent{});
}

TEST_F(CharacterControllerTests, DialogState_MoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_MoveForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_MoveBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_MoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_MoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_WalkChangeStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_SprintStateChangeEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_EndForwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_EndBackwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_RotateLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_RotateRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_RotateTargetEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_EndRotationEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_AttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_EndAttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_DeathEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_WeaponStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_EquipEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_DisarmEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_DrawArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_ReloadArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_AimStopEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_CrouchChangeStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_StartFallingEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_GroundDetectionEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_HurtEvent)
{
    prepareState(*this);
    tiggerAndExpect<HurtEvent>({sut_.animationClipsNames_.disarmed.posture.stand.hurt});
}

TEST_F(CharacterControllerTests, DialogState_AnimationPlayEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DialogState_StartDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
TEST_F(CharacterControllerTests, DialogState_EndDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}