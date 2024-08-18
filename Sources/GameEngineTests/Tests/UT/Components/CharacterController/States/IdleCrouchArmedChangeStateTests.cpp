#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<CrouchChangeStateEvent>({test.sut_.animationClipsNames_.disarmed.posture.crouched.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.equip, test.sut_.animationClipsNames_.armed.posture.crouched.idle});
}
}  // namespace

TEST_F(CharacterControllerTests, IdleCrouchArmedChangeState_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.equip});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(0);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.idle});
}
TEST_F(CharacterControllerTests, IdleCrouchArmedChangeState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.posture.crouched.death});
}
TEST_F(CharacterControllerTests, IdleCrouchArmedChangeState_EquipEndStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<EquipEndStateEvent>({sut_.animationClipsNames_.armed.posture.crouched.idle},
                                        {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, IdleCrouchArmedChangeState_DisarmEndStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<DisarmEndStateEvent>({sut_.animationClipsNames_.disarmed.posture.crouched.idle},
                                         {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, IdleCrouchArmedChangeState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.movement.crouch.forward});
}
TEST_F(CharacterControllerTests, IdleCrouchArmedChangeState_MoveBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.movement.crouch.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
}
TEST_F(CharacterControllerTests, IdleCrouchArmedChangeState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_WALK_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.movement.crouch.forward});
    expectRootboneRotation(VECTOR_LEFT);
}
TEST_F(CharacterControllerTests, IdleCrouchArmedChangeState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_WALK_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.movement.crouch.forward});
    expectRootboneRotation(VECTOR_RIGHT);
}
TEST_F(CharacterControllerTests, IdleCrouchArmedChangeState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.posture.crouched.rotate.left});
}
TEST_F(CharacterControllerTests, IdleCrouchArmedChangeState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.posture.crouched.rotate.right});
}
TEST_F(CharacterControllerTests, IdleCrouchArmedChangeState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(
        RotateTargetEvent{targetRotation.value_},
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.armed.posture.crouched.rotate.left});
}
// TEST_F(CharacterControllerTests, IdleCrouchArmedChangeState_DrawArrowEvent)
//{
//    prepareState(*this);
//    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.equip,
//    sut_.animationClipsNames_.armed.posture.crouched.idle}); Update(ADVANCED_TIME_CLIP_TIME);
//    Update(ADVANCED_TIME_TRANSITION_TIME);
//    expectAnimsToBeSet({sut_.animationClipsNames_.aim.draw});
//}
// TEST_F(CharacterControllerTests, IdleCrouchArmedChangeState_AimStopEvent)
//{
//    prepareState(*this);
//    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.equip,
//    sut_.animationClipsNames_.armed.posture.crouched.idle}); tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.equip,
//    sut_.animationClipsNames_.armed.posture.crouched.idle}); Update(ADVANCED_TIME_CLIP_TIME);
//    Update(ADVANCED_TIME_TRANSITION_TIME);
//    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.crouched.idle});
//}
