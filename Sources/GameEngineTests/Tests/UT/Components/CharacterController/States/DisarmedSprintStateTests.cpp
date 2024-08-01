#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});

    test.expectForwardVelocity(DEFAULT_RUN_SPEED);
    test.tiggerAndExpect<MoveForwardEvent>({test.sut_.animationClipsNames_.disarmed.run.forward});
    test.expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    test.tiggerAndExpect<SprintStateChangeEvent>({test.sut_.animationClipsNames_.disarmed.sprint});
}
}  // namespace

TEST_F(CharacterControllerTests, DisarmedSprintState_CrouchChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.disarmed.crouch.forward});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_DrawArrowEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip},
                                    {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
    Update(ADVANCED_TIME_CLIP_TIME / 2.f);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.drawArrow});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_MoveForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.sprint});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_MoveBackwardEvent)
{
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.idle});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_SPRINT_SPEED, 0.0, DEFAULT_SPRINT_SPEED));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.disarmed.sprint}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, DisarmedSprintState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_RIGHT, vec3(DEFAULT_SPRINT_SPEED, 0.0, DEFAULT_SPRINT_SPEED));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.disarmed.sprint}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_RIGHT);
}
TEST_F(CharacterControllerTests, DisarmedSprintState_EndBackwardMoveEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.run.forward}, {0});
    expectRootboneRotation(VECTOR_BACKWARD);
    expectForwardVelocity(-DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.sprint});
    expectRootboneRotation(VECTOR_BACKWARD);
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.disarmed.idle});
    expectRootboneRotation(VECTOR_BACKWARD);
}
TEST_F(CharacterControllerTests, DisarmedSprintState_EndMoveLeftEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0, 0));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.disarmed.run.forward}, {DEFAULT_MOVING_CHANGE_DIR_SPEED});
    expectRootboneRotation(VECTOR_LEFT);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_SPRINT_SPEED, 0, 0));
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.sprint});
    expectRootboneRotation(VECTOR_LEFT);
    tiggerAndExpect<EndMoveLeftEvent>({sut_.animationClipsNames_.disarmed.idle});
    expectRootboneRotation(VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, DisarmedSprintState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.death});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_WalkChangeStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.walk.forward});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_SprintStateChangeEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.run.forward});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_WeaponStateEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip},
                                      {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_EndForwardMoveEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.idle});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_RotateLeftEvent)
{
    prepareState(*this);
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.disarmed.sprint});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.disarmed.sprint});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_RotateTargetEvent)
{
    prepareState(*this);
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.disarmed.sprint});
}
TEST_F(CharacterControllerTests, DisarmedSprintState_AimStopEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip},
                                    {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME / 2.f});
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.run.forward},
                                  {ADVANCED_TIME_CLIP_TIME / 2.f, ADVANCED_TIME_TRANSITION_TIME});
}
