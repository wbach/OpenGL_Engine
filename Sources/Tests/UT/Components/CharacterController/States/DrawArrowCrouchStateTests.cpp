#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    EXPECT_CALL(test.physicsApiMock_, SetRotation(test.rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<CrouchChangeStateEvent>({test.sut_.animationClipsNames_.armed.posture.crouched.idle});
    test.tiggerAndExpect<DrawArrowEvent>(
        {test.sut_.animationClipsNames_.aim.draw, test.sut_.animationClipsNames_.armed.posture.crouched.idle});
}
}  // namespace

TEST_F(CharacterControllerTests, DrawArowCrouchState_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.aim.draw});
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_RotateLeft)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.posture.crouched.rotate.left});
}
TEST_F(CharacterControllerTests, DrawArowCrouchState_RotateRight)
{
    prepareState(*this);
    tiggerAndExpect<RotateRightEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.posture.crouched.rotate.right});
}
TEST_F(CharacterControllerTests, DrawArowCrouchState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.posture.crouched.rotate.right});
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarm.front(), sut_.animationClipsNames_.armed.posture.crouched.idle});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.crouched.idle});
}
TEST_F(CharacterControllerTests, DrawArowCrouchState_AimStartEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStartEvent>({sut_.animationClipsNames_.aim.idle, sut_.animationClipsNames_.armed.posture.crouched.idle});
}
TEST_F(CharacterControllerTests, DrawArowCrouchState_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.posture.crouched.idle});
}
TEST_F(CharacterControllerTests, DrawArowCrouchState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.crouched.death});
}
TEST_F(CharacterControllerTests, DrawArowCrouchState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent>(
        {sut_.animationClipsNames_.armed.movement.crouch.forward, sut_.animationClipsNames_.aim.draw});
}
TEST_F(CharacterControllerTests, DrawArowCrouchState_WalkBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.armed.movement.crouch.backward, sut_.animationClipsNames_.aim.draw});
}
TEST_F(CharacterControllerTests, DrawArowCrouchState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.armed.movement.crouch.left, sut_.animationClipsNames_.aim.draw});
}
TEST_F(CharacterControllerTests, DrawArowCrouchState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_LEFT_RIGHT_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.armed.movement.crouch.right, sut_.animationClipsNames_.aim.draw});
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_MoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_MoveBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_WalkChangeStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_SprintStateChangeEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_EndForwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_EndBackwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_RotateLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_RotateRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_EndRotationEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_AttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_EndAttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_EquipEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_DisarmEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_DrawArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DrawArowCrouchState_ReloadArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
