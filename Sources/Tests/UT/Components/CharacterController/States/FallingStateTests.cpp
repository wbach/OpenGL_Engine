#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.tiggerAndExpect<StartFallingEvent>({test.sut_.animationClipsNames_.disarmed.falling});
}

TEST_F(CharacterControllerTests, FallingStateTests_EndForwardMoveEvent)
{
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    expectForwardVelocity(DEFAULT_RUN_SPEED);

    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    tiggerAndExpect<StartFallingEvent>({sut_.animationClipsNames_.disarmed.falling},
                                       {ADVANCED_TIME_TRANSITION_TIME, 0.2f, ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.falling});
    expectNoMove();
    tiggerAndExpect<GroundDetectionEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}
TEST_F(CharacterControllerTests, FallingStateTests_DuringForwardMoveEvent)
{
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    expectForwardVelocity(DEFAULT_RUN_SPEED);

    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    tiggerAndExpect<StartFallingEvent>({sut_.animationClipsNames_.disarmed.falling},
                                       {ADVANCED_TIME_TRANSITION_TIME, 0.2f, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<GroundDetectionEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, FallingStateTests_MoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_MoveBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_WalkChangeStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_SprintStateChangeEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_EndBackwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_RotateTargetEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_EndRotationEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_AttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_EndAttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_DeathEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_WeaponStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_EquipEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_DisarmEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_DrawArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_ReloadArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_AimStopEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_CrouchChangeStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_StartFallingEvent)
{
    prepareState(*this);
    expectIgnoreEvent<StartFallingEvent>();
}

TEST_F(CharacterControllerTests, FallingStateTests_GroundDetectionEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_HurtEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_AnimationPlayEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_StartDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, FallingStateTests_EndDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
}  // namespace
