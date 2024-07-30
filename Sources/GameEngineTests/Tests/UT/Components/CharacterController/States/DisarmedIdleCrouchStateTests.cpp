#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<CrouchChangeStateEvent>({test.sut_.animationClipsNames_.disarmed.crouchIdle});
}
}  // namespace

//TEST_F(CharacterControllerTests, DisarmedIdleCrouchState_DrawArrowEvent) TO DO
//{
//    prepareState(*this);
//    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.equip}, {ADVANCED_TIME_TRANSITION_TIME});
//    Update(ADVANCED_TIME_CLIP_TIME);
//    Update(ADVANCED_TIME_TRANSITION_TIME);
//    expectAnimsToBeSet({sut_.animationClipsNames_.drawArrow});
//}

//TEST_F(CharacterControllerTests, DisarmedIdleCrouchState_AimStopDuringDrawArrowEvent) TO DO
//{
//    prepareState(*this);
//    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.equip}, {ADVANCED_TIME_TRANSITION_TIME});

//    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.equip});
//    Update(ADVANCED_TIME_CLIP_TIME);
//    Update(ADVANCED_TIME_TRANSITION_TIME);
//    expectAnimsToBeSet({sut_.animationClipsNames_.armed.idle});
//}

//TEST_F(CharacterControllerTests, DisarmedIdleCrouchState_WeaponStateEvent) TO DO
//{
//    prepareState(*this);
//    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.equip}, {ADVANCED_TIME_TRANSITION_TIME});
//    Update(ADVANCED_TIME_CLIP_TIME);
//    Update(ADVANCED_TIME_TRANSITION_TIME);
//}

TEST_F(CharacterControllerTests, DisarmedIdleCrouchState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.death});
}

TEST_F(CharacterControllerTests, DisarmedIdleCrouchState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_FORWARD);
}

TEST_F(CharacterControllerTests, DisarmedIdleCrouchState_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, DisarmedIdleCrouchState_MoveLeftEvent)
{
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, DisarmedIdleCrouchState_MoveRightEvent)
{
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, DisarmedIdleCrouchState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.disarmed.rotateLeft});
}

TEST_F(CharacterControllerTests, DisarmedIdleCrouchState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.disarmed.rotateRight});
}

TEST_F(CharacterControllerTests, DisarmedIdleCrouchState_RotateTargetEvent)
{
    prepareState(*this);

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.disarmed.rotateLeft});
}

TEST_F(CharacterControllerTests, DisarmedIdleCrouchState_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().name},
                                 {ADVANCED_TIME_TRANSITION_TIME});
}

TEST_F(CharacterControllerTests, DisarmedIdleCrouchState_CrouchChangeStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.disarmed.idle}, {ADVANCED_TIME_TRANSITION_TIME});
}
