#include "../CharacterControllerTests.h"
#include "Components/Controllers/CharacterController/CharacterControllerEvents.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.expectRotationRight(ADVANCED_TIME_TRANSITION_TIME);
    test.tiggerAndExpect<RotateRightEvent>({test.sut_.animationClipsNames_.armed.posture.stand.rotate.right});
    test.tiggerAndExpect<CrouchChangeStateEvent>({test.sut_.animationClipsNames_.armed.posture.crouched.rotate.right});
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedCrouchRotate_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.right});
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.aim.draw, sut_.animationClipsNames_.armed.posture.crouched.rotate.right});
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.crouched.death});
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarmed.posture.crouched.rotate.right, sut_.animationClipsNames_.disarm.front()});
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.armed.movement.crouch.forward});
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_MoveBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.armed.movement.crouch.forward});
    expectRotationRight(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.armed.movement.crouch.forward});
    expectRotationRight(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.armed.movement.crouch.forward});
    expectRotationRight(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.armed.posture.crouched.rotate.right});
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.armed.posture.crouched.rotate.left});
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_RotateTargetEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.armed.posture.crouched.rotate.right});

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.armed.posture.crouched.rotate.right});
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.armed.posture.crouched.idle});
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_MoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_WalkChangeStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_SprintStateChangeEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_EndForwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_EndBackwardMoveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_EndMoveLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_EndMoveRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_RotateRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_JumpEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_JumpConfirmEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_DodgeDiveEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_DodgeForwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_DodgeBackwardEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_DodgeLeftEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_DodgeRightEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_DodgeEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_AttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_EndAttackEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_EquipEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_DisarmEndStateEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_ReloadArrowEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_AimStartEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_AimStopEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_StartFallingEvent)
{
    prepareState(*this);
    tiggerAndExpect<StartFallingEvent>({sut_.animationClipsNames_.armed.falling});
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_GroundDetectionEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_HurtEvent)
{
    prepareState(*this);
    tiggerAndExpect<HurtEvent>({sut_.animationClipsNames_.armed.posture.crouched.hurt});
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_AnimationPlayEndEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_ChangeAnimEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_StartDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, ArmedCrouchRotate_EndDialogEvent)
{
    prepareState(*this);
    /* TODO: Implement test */
}
