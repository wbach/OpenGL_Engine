#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    for (auto& state : test.sut_.animationClipsNames_.armed.attack)
    {
        state.stateType = GameEngine::Components::PlayStateType::run;
    }

    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<WeaponStateEvent>(
        {test.sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<MoveForwardEvent>({test.sut_.animationClipsNames_.armed.movement.run.forward});

    test.tiggerAndExpect<AttackEvent>(
        {test.sut_.animationClipsNames_.armed.attack.front().name, test.sut_.animationClipsNames_.armed.movement.run.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedAttackAndRunState_CrouchChangeStateEvent)
{
    prepareState(*this);
    // queue DrawArrowEvent
    tiggerAndExpect<CrouchChangeStateEvent>(
        {sut_.animationClipsNames_.armed.attack.front().name, sut_.animationClipsNames_.armed.movement.run.forward});

    // wait until attack finish
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.crouch.forward});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunState_DrawArrowEvent)
{
    prepareState(*this);
    // queue DrawArrowEvent
    tiggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.armed.attack.front().name, sut_.animationClipsNames_.armed.movement.run.forward});

    // wait until attack finish
    Update(ADVANCED_TIME_CLIP_TIME);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.walk.forward, sut_.animationClipsNames_.aim.draw});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunState_MoveForwardEvent)
{
    prepareState(*this);
    for (auto& state : sut_.animationClipsNames_.armed.attack)
    {
        state.stateType = GameEngine::Components::PlayStateType::run;
    }

    const auto& attackClipName = sut_.animationClipsNames_.armed.attack.front().name;

    tiggerAndExpect<EndForwardMoveEvent>({attackClipName});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({attackClipName, sut_.animationClipsNames_.armed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_BACKWARD);
    expectNoMove();
    tiggerAndExpect<MoveForwardEvent>({attackClipName});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunState_MoveBackwardEvent)
{
    const auto& attackClipName = sut_.animationClipsNames_.armed.attack.front().name;
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({attackClipName});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunState_MoveLeftEvent_EndMoveLeftEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.armed.attack.front().name;

    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, DEFAULT_RUN_SPEED));
    tiggerAndExpect<MoveLeftEvent>({attackClipName, sut_.animationClipsNames_.armed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_LEFT);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<EndForwardMoveEvent>({attackClipName, sut_.animationClipsNames_.armed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_LEFT);
    tiggerAndExpect<EndMoveLeftEvent>({attackClipName}, {0});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunState_MoveRightEvent_EndMoveRightEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.armed.attack.front().name;

    expectVelocity(VECTOR_FORWARD + VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, DEFAULT_RUN_SPEED));
    tiggerAndExpect<MoveRightEvent>({attackClipName, sut_.animationClipsNames_.armed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_RIGHT);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<EndForwardMoveEvent>({attackClipName, sut_.animationClipsNames_.armed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_RIGHT);
    tiggerAndExpect<EndMoveRightEvent>({attackClipName}, {0});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.armed.posture.stand.death});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunState_WalkChangeStateEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.armed.attack.front().name;
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({attackClipName, sut_.animationClipsNames_.armed.movement.walk.forward});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunState_WeaponStateEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.armed.attack.front().name;
    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.armed.movement.run.forward, attackClipName});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward, sut_.animationClipsNames_.disarm});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunState_EndForwardMoveEvent)
{
    prepareState(*this);
    const auto& clip = sut_.animationClipsNames_.armed.attack.front();
    expectNoMove();
    tiggerAndExpect<EndForwardMoveEvent>({clip.name});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunState_EndBackwardMoveEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().name;
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({clipName}, {0});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<EndBackwardMoveEvent>({clipName, sut_.animationClipsNames_.armed.movement.run.forward}, {0});
    tiggerAndExpect<EndForwardMoveEvent>({clipName}, {0});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunState_RotateLeftEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().name;
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>({clipName, sut_.animationClipsNames_.armed.movement.run.forward});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunState_RotateRightEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().name;
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({clipName, sut_.animationClipsNames_.armed.movement.run.forward});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunState_RotateTargetEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().name;
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {clipName, sut_.animationClipsNames_.armed.movement.run.forward});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunState_SprintStateChangeEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.armed.attack.front().name;
    tiggerAndExpect<SprintStateChangeEvent>({clipName, sut_.animationClipsNames_.armed.movement.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.sprint});
}

TEST_F(CharacterControllerTests, ArmedAttackAndRunState_AttackEvent)
{
    prepareState(*this);
    const auto& clip1 = sut_.animationClipsNames_.armed.attack[0];
    auto& clip2       = sut_.animationClipsNames_.armed.attack[1];
    const auto& clip3 = sut_.animationClipsNames_.armed.attack[2];
    clip2.stateType   = GameEngine::Components::PlayStateType::idle;

    tiggerAndExpect<AttackEvent>({clip1.name, sut_.animationClipsNames_.armed.movement.run.forward});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<AttackEvent>({clip2.name});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({clip3.name, sut_.animationClipsNames_.armed.movement.run.forward});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward});
}
TEST_F(CharacterControllerTests, ArmedAttackAndRunState_AttackEventWhenRunBackwardAnimOnly)
{
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.armed.posture.stand.idle},
                                      {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward});

    expectNoMove();
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.armed.attack.front().name});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward});
}
TEST_F(CharacterControllerTests, ArmedAttackAndRunState_MoveForwardAndLeftEvent)
{
    const auto& clip = sut_.animationClipsNames_.armed.attack.front();
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, DEFAULT_RUN_SPEED));
    tiggerAndExpect<MoveLeftEvent>({clip.name, sut_.animationClipsNames_.armed.movement.run.forward});
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<EndForwardMoveEvent>({clip.name, sut_.animationClipsNames_.armed.movement.run.forward}, {0});
    tiggerAndExpect<EndMoveLeftEvent>({clip.name}, {0});
}
TEST_F(CharacterControllerTests, ArmedAttackAndRunState_PR_MoveStopAndGoDuringAttack)
{
    for (auto& state : sut_.animationClipsNames_.armed.attack)
    {
        state.stateType = GameEngine::Components::PlayStateType::run;
    }
    const auto& attackClip = sut_.animationClipsNames_.armed.attack.front();
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>({attackClip.name}, {0});
    tiggerAndExpect<MoveForwardEvent>({attackClip.name, sut_.animationClipsNames_.armed.movement.run.forward}, {0});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.idle}, {0});
}
