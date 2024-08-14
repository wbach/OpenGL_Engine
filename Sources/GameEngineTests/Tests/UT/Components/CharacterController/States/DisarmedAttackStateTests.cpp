#include "../CharacterControllerTests.h"

TEST_F(CharacterControllerTests, DisarmedAttackState_DrawArrowEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().name}, {ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.equip}, {ADVANCED_TIME_TRANSITION_TIME});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.drawArrow});
}
TEST_F(CharacterControllerTests, DisarmedAttackState_AimStopDuringDrawArrowEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().name}, {ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.equip}, {ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, DisarmedAttackState_WeaponStateEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().name}, {ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.equip}, {ADVANCED_TIME_TRANSITION_TIME});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
}

TEST_F(CharacterControllerTests, DisarmedAttackState_DeathEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().name}, {ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.death});
}

TEST_F(CharacterControllerTests, DisarmedAttackState_MoveForwardEvent_idle)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    auto& clip = sut_.animationClipsNames_.disarmed.attack.front();
    tiggerAndExpect<AttackEvent>({clip.name}, {ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<MoveForwardEvent>({clip.name}, {ADVANCED_TIME_TRANSITION_TIME});

    expectNoMove();
    expectAnimsToBeSet({clip.name});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_FORWARD);
}

TEST_F(CharacterControllerTests, DisarmedAttackState_MoveForwardEvent_walk)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    auto& clip     = sut_.animationClipsNames_.disarmed.attack.front();
    clip.stateType = GameEngine::Components::PlayStateType::walk;
    tiggerAndExpect<AttackEvent>({clip.name}, {ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.walk.forward, clip.name});
    expectRootboneRotation(VECTOR_FORWARD);
}

TEST_F(CharacterControllerTests, DisarmedAttackState_MoveForwardEvent_run)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    auto& clip     = sut_.animationClipsNames_.disarmed.attack.front();
    clip.stateType = GameEngine::Components::PlayStateType::run;
    tiggerAndExpect<AttackEvent>({clip.name}, {ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.run.forward, clip.name});
    expectRootboneRotation(VECTOR_FORWARD);
}

TEST_F(CharacterControllerTests, DisarmedAttackState_RunBackwardEvent_idle)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    auto& clip = sut_.animationClipsNames_.disarmed.attack.front();
    tiggerAndExpect<AttackEvent>({clip.name}, {ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<MoveBackwardEvent>({clip.name}, {ADVANCED_TIME_TRANSITION_TIME});

    expectNoMove();
    expectAnimsToBeSet({clip.name});

    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, DisarmedAttackState_MoveLeftEvent_idle)
{
    auto& clip = sut_.animationClipsNames_.disarmed.attack.front();
    tiggerAndExpect<AttackEvent>({clip.name}, {ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<MoveLeftEvent>({clip.name}, {ADVANCED_TIME_TRANSITION_TIME});
    expectNoMove();
    expectAnimsToBeSet({clip.name});

    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, DisarmedAttackState_MoveRightEvent_idle)
{
    auto& clip = sut_.animationClipsNames_.disarmed.attack.front();
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().name}, {ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<MoveRightEvent>({clip.name}, {ADVANCED_TIME_TRANSITION_TIME});
    expectNoMove();
    expectAnimsToBeSet({clip.name});

    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});
    expectRootboneRotation(VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, DisarmedAttackState_RotateLeftEvent)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<AttackEvent>({disarmedAttackClip1->name}, {ADVANCED_TIME_TRANSITION_TIME});

    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>({disarmedAttackClip1->name, sut_.animationClipsNames_.disarmed.rotateLeft});
}

TEST_F(CharacterControllerTests, DisarmedAttackState_RotateRightEvent)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<AttackEvent>({disarmedAttackClip1->name}, {ADVANCED_TIME_TRANSITION_TIME});

    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({disarmedAttackClip1->name, sut_.animationClipsNames_.disarmed.rotateRight});
}

TEST_F(CharacterControllerTests, DisarmedAttackState_RotateTargetEvent)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<AttackEvent>({disarmedAttackClip1->name}, {ADVANCED_TIME_TRANSITION_TIME});

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {disarmedAttackClip1->name, sut_.animationClipsNames_.disarmed.rotateLeft});
}
TEST_F(CharacterControllerTests, DisarmedAttackState_EndAttackEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().name}, {ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<EndAttackEvent>({sut_.animationClipsNames_.disarmed.idle}, {ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, DisarmedAttackState_AttackEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    const auto& clip1 = sut_.animationClipsNames_.disarmed.attack[0];
    const auto& clip3 = sut_.animationClipsNames_.disarmed.attack[2];
    auto& clip2       = sut_.animationClipsNames_.disarmed.attack[1];
    clip2.stateType   = GameEngine::Components::PlayStateType::run;

    tiggerAndExpect<AttackEvent>({clip1.name}, {ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<AttackEvent>({clip1.name}, {0});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<AttackEvent>({clip2.name}, {0});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    tiggerAndExpect<AttackEvent>({clip3.name}, {0});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({clip1.name});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}
