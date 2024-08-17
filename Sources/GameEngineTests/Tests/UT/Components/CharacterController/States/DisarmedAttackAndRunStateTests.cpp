#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    for (auto& state : test.sut_.animationClipsNames_.disarmed.attack)
    {
        state.stateType = GameEngine::Components::PlayStateType::run;
    }

    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    test.expectForwardVelocity(DEFAULT_RUN_SPEED);

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.tiggerAndExpect<MoveForwardEvent>({test.sut_.animationClipsNames_.disarmed.movement.run.forward});
    test.tiggerAndExpect<AttackEvent>({test.sut_.animationClipsNames_.disarmed.attack.front().name,
                                       test.sut_.animationClipsNames_.disarmed.movement.run.forward});
}
}  // namespace

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_CrouchChangeStateEvent)
{
    prepareState(*this);
    // queue DrawArrowEvent
    tiggerAndExpect<CrouchChangeStateEvent>(
        {sut_.animationClipsNames_.disarmed.attack.front().name, sut_.animationClipsNames_.disarmed.movement.run.forward});

    // wait until attack finish
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.crouch.forward});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_DrawArrowEvent)
{
    prepareState(*this);
    // queue DrawArrowEvent
    tiggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.disarmed.attack.front().name, sut_.animationClipsNames_.disarmed.movement.run.forward});

    // wait until attack finish
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip});

    Update(ADVANCED_TIME_CLIP_TIME);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.walk.forward, sut_.animationClipsNames_.aim.draw});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_AimStopDuringDrawArrowEvent)
{
    prepareState(*this);
    // queue DrawArrowEvent
    tiggerAndExpect<DrawArrowEvent>(
        {sut_.animationClipsNames_.disarmed.attack.front().name, sut_.animationClipsNames_.disarmed.movement.run.forward});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    tiggerAndExpect<AimStopEvent>(
        {sut_.animationClipsNames_.disarmed.attack.front().name, sut_.animationClipsNames_.disarmed.movement.run.forward});

    // Extra check
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_MoveForwardEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.disarmed.attack.front().name;

    tiggerAndExpect<EndForwardMoveEvent>({attackClipName});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({attackClipName, sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_BACKWARD);
    expectNoMove();
    tiggerAndExpect<MoveForwardEvent>({attackClipName});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_MoveBackwardEvent)
{
    const auto& attackClipName = sut_.animationClipsNames_.disarmed.attack.front().name;
    prepareState(*this);
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({attackClipName});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_MoveLeftEvent_EndMoveLeftEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.disarmed.attack.front().name;

    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, DEFAULT_RUN_SPEED));
    tiggerAndExpect<MoveLeftEvent>({attackClipName, sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_LEFT);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<EndForwardMoveEvent>({attackClipName, sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_LEFT);
    tiggerAndExpect<EndMoveLeftEvent>({attackClipName}, {0});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_MoveRightEvent_EndMoveRightEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.disarmed.attack.front().name;

    expectVelocity(VECTOR_FORWARD + VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, DEFAULT_RUN_SPEED));
    tiggerAndExpect<MoveRightEvent>({attackClipName, sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_FORWARD + VECTOR_RIGHT);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<EndForwardMoveEvent>({attackClipName, sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    expectRootboneRotation(VECTOR_RIGHT);
    tiggerAndExpect<EndMoveRightEvent>({attackClipName}, {0});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.posture.stand.death});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_WalkChangeStateEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.disarmed.attack.front().name;
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({attackClipName, sut_.animationClipsNames_.disarmed.movement.walk.forward});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_WeaponStateEvent)
{
    prepareState(*this);
    const auto& attackClipName = sut_.animationClipsNames_.disarmed.attack.front().name;
    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward, attackClipName});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_EndForwardMoveEvent)
{
    prepareState(*this);
    const auto& clip = sut_.animationClipsNames_.disarmed.attack.front();
    expectNoMove();
    tiggerAndExpect<EndForwardMoveEvent>({clip.name});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_EndBackwardMoveEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.disarmed.attack.front().name;
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({clipName}, {0});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<EndBackwardMoveEvent>({clipName, sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    tiggerAndExpect<EndForwardMoveEvent>({clipName}, {0});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_RotateLeftEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.disarmed.attack.front().name;
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>({clipName, sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_RotateRightEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.disarmed.attack.front().name;
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({clipName, sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_RotateTargetEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.disarmed.attack.front().name;
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {clipName, sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_SprintStateChangeEvent)
{
    prepareState(*this);
    const auto& clipName = sut_.animationClipsNames_.disarmed.attack.front().name;
    tiggerAndExpect<SprintStateChangeEvent>({clipName, sut_.animationClipsNames_.disarmed.movement.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.sprint});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_AttackEventSecondClipIsIdleOnly)
{
    prepareState(*this);
    auto& clip1     = sut_.animationClipsNames_.disarmed.attack[0];
    clip1.stateType = GameEngine::Components::PlayStateType::run;
    auto& clip2     = sut_.animationClipsNames_.disarmed.attack[1];
    clip2.stateType = GameEngine::Components::PlayStateType::idle;

    tiggerAndExpect<AttackEvent>({clip1.name, sut_.animationClipsNames_.disarmed.movement.run.forward});

    // Wait for attack anim 1 finished, and second started
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({clip2.name});

    // back to run after second idleOnly clip finished
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_AttackEvent)
{
    prepareState(*this);
    const auto& clip1 = sut_.animationClipsNames_.disarmed.attack[0];
    const auto& clip3 = sut_.animationClipsNames_.disarmed.attack[2];
    auto& clip2       = sut_.animationClipsNames_.disarmed.attack[1];
    clip2.stateType   = GameEngine::Components::PlayStateType::idle;

    tiggerAndExpect<AttackEvent>({clip1.name, sut_.animationClipsNames_.disarmed.movement.run.forward});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<AttackEvent>({clip2.name});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({clip3.name, sut_.animationClipsNames_.disarmed.movement.run.forward});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_AttackEventWhenRunBackward)
{
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});

    expectNoMove();
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().name});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_MoveForwardAndLeftEvent)
{
    const auto& clip = sut_.animationClipsNames_.disarmed.attack.front();
    prepareState(*this);
    expectVelocity(VECTOR_FORWARD + VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, DEFAULT_RUN_SPEED));
    tiggerAndExpect<MoveLeftEvent>({clip.name, sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0.0));
    tiggerAndExpect<EndForwardMoveEvent>({clip.name, sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    tiggerAndExpect<EndMoveLeftEvent>({clip.name}, {0});
}
TEST_F(CharacterControllerTests, DisarmedAttackAndRunState_PR_MoveStopAndGoDuringAttack)
{
    const auto& attackClip = sut_.animationClipsNames_.disarmed.attack.front();
    prepareState(*this);
    tiggerAndExpect<EndForwardMoveEvent>({attackClip.name}, {0});
    tiggerAndExpect<MoveForwardEvent>({attackClip.name, sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle}, {0});
}
