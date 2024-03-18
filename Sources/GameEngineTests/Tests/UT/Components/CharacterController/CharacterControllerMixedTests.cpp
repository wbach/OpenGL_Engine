#include "CharacterControllerTests.h"

TEST_F(CharacterControllerTests, Mixed_DisarmedState_IdleToRunForward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    sut_.handleEvent(MoveForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});

    sut_.handleEvent(EndForwardMoveEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedState_IdleToRunBackward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, -DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    sut_.handleEvent(MoveBackwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectRootboneRotation(VECTOR_BACKWARD);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});

    sut_.handleEvent(EndBackwardMoveEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedState_IdleToRunForwardAndBackward)
{
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    sut_.handleEvent(MoveForwardEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});

    expectNoMove();
    sut_.handleEvent(MoveBackwardEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    sut_.handleEvent(EndBackwardMoveEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedToArmedStateIdle)
{
    sut_.handleEvent(WeaponStateEvent{});

    // Animation transition : None -> Armed idle state
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.equip});

    // Animation : EquipAnim
    Update(ADVANCED_TIME_CLIP_TIME);

    // State need to change
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_RunForwardDuringDisarmedToArmedState)
{
    sut_.handleEvent(WeaponStateEvent{});

    // Animation transition : None -> Armed idle state (
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.equip});

    // Animation : EquipAnim
    Update(DUMMY_CLIP_LENGTH / 2.f);

    sut_.handleEvent(MoveForwardEvent{});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});

    Update(DUMMY_CLIP_LENGTH / 2.f);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward});
}

TEST_F(CharacterControllerTests, Mixed_EquipWeaponDuringDisarmedRunForward)
{
    vec3 runningVelocity(0.0, 0.0, DEFAULT_RUN_SPEED);

    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, runningVelocity)).Times(AtLeast(1));

    sut_.handleEvent(MoveForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});

    sut_.handleEvent(WeaponStateEvent{});

    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(runningVelocity));

    // PlayMixedAnimation with equip
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});

    // Animation : Tirgger EquipAnim end, and notify state
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward});
    Update(ADVANCED_TIME_TRANSITION_TIME);
}

TEST_F(CharacterControllerTests, Mixed_DisarmWeaponDuringArmedRunForward)
{
    vec3 runningVelocity(0.0, 0.0, DEFAULT_RUN_SPEED);

    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, runningVelocity)).Times(AtLeast(1));

    sut_.handleEvent(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);

    sut_.handleEvent(MoveForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward});

    sut_.handleEvent(WeaponStateEvent{});

    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(runningVelocity));

    // PlayMixedAnimation with equip
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward, sut_.animationClipsNames_.disarm});

    // Animation : Tirgger EquipAnim end, and notify state
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});
    Update(ADVANCED_TIME_TRANSITION_TIME);
}

TEST_F(CharacterControllerTests, Mixed_EquipWeaponDuringRotateLeftState)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    sut_.handleEvent(RotateLeftEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.rotateLeft});

    sut_.handleEvent(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.rotateLeft, sut_.animationClipsNames_.equip});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.rotateLeft});

    sut_.handleEvent(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.rotateLeft, sut_.animationClipsNames_.disarm});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.rotateLeft});

    sut_.handleEvent(EndRotationEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_EquipWeaponDuringRotateRightState)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    sut_.handleEvent(RotateRightEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.rotateRight});

    sut_.handleEvent(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.rotateRight, sut_.animationClipsNames_.equip});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.rotateRight});

    sut_.handleEvent(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.rotateRight, sut_.animationClipsNames_.disarm});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.rotateRight});

    sut_.handleEvent(EndRotationEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedStateToArmed_RunForwardAndRotateLeft)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    sut_.handleEvent(MoveForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});

    sut_.handleEvent(RotateLeftEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});

    sut_.handleEvent(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward});

    sut_.handleEvent(EndRotationEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward});

    sut_.handleEvent(EndForwardMoveEvent{});
    ;

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedStateToArmed_RunForwardAndRotateRight)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    sut_.handleEvent(MoveForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});

    sut_.handleEvent(RotateRightEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});

    sut_.handleEvent(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward});

    sut_.handleEvent(EndRotationEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward});

    sut_.handleEvent(EndForwardMoveEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedState_RunForwardAndRotateLeftAndChangeWeaponState)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    sut_.handleEvent(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.equip});

    sut_.handleEvent(MoveForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});

    sut_.handleEvent(RotateLeftEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});

    sut_.handleEvent(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward, sut_.animationClipsNames_.disarm});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});

    sut_.handleEvent(EndForwardMoveEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.rotateLeft});

    sut_.handleEvent(EndRotationEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}
TEST_F(CharacterControllerTests, Mixed_DisamredToArmedState_DuringEquipRunForwardAndRotateLeftAndChangeWeaponState)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    sut_.handleEvent(WeaponStateEvent{});
    sut_.handleEvent(MoveForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedStopMovingDuringEuip)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    sut_.handleEvent(MoveForwardEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});
    sut_.handleEvent(WeaponStateEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    sut_.handleEvent(EndForwardMoveEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.equip});
    sut_.handleEvent(WeaponStateEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisamredDuringEquip)
{
    sut_.handleEvent(WeaponStateEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.equip});
    sut_.handleEvent(WeaponStateEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarm});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisamredDuringEquipTransition)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    sut_.handleEvent(WeaponStateEvent{});

    tiggerAndExpect<WeaponStateEvent, IdleArmedChangeState>({sut_.animationClipsNames_.disarm});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_IdleToWalkToIdleState)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<WalkChangeStateEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_IdleChangeWalkEventDoNothing)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WalkChangeStateEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_RunToWalkToRunState)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
}

TEST_F(CharacterControllerTests, Mixed_WalkAndRotateState)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_WALK_SPEED)))
        .Times(AtLeast(1));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    tiggerAndExpect<MoveForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<WalkChangeStateEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
    tiggerAndExpect<RotateRightEvent, DisarmedWalkAndRotateState>({sut_.animationClipsNames_.disarmed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateRight});
}

TEST_F(CharacterControllerTests, Mixed_RotateAndWalkState)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_WALK_SPEED)))
        .Times(AtLeast(1));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    tiggerAndExpect<RotateRightEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateRight});
    tiggerAndExpect<MoveForwardEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<WalkChangeStateEvent, DisarmedWalkAndRotateState>({sut_.animationClipsNames_.disarmed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateRight});
}

TEST_F(CharacterControllerTests, Mixed_RunAndRotateToWalkAndRotate)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<RotateRightEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, DisarmedWalkAndRotateState>(
        {sut_.animationClipsNames_.disarmed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateRight});
}

TEST_F(CharacterControllerTests, Mixed_RunAndRotateToWalkAndRotateToWalkForward)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<RotateRightEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.forward});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, DisarmedWalkAndRotateState>(
        {sut_.animationClipsNames_.disarmed.walk.forward});
    tiggerAndExpect<EndRotationEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedIdleToWalkForwardBackward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndBackwardMoveEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedIdleToWalkBackwardForward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});

    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.backward});
    expectNoMove();
    tiggerAndExpect<MoveForwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<EndForwardMoveEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedIdleToWalkForward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedIdleToWalkBackward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedIdleToWalkForwardBackward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.idle});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndBackwardMoveEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedIdleDoNothingIfWalkChangeEvent)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<WalkChangeStateEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedRunToWalkForward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.idle});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndBackwardMoveEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedRunToWalkForwardToRun)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedRunToWalkBackwardToRun)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward});
    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    tiggerAndExpect<EndBackwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedRunAndRotateForwardToWalkAndRotateForwardAndRotation)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<RotateLeftEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateLeft});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});
    tiggerAndExpect<RotateRightEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});
    tiggerAndExpect<RotateLeftEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});
    tiggerAndExpect<RotateRightEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});

    tiggerAndExpect<EndRotationEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedRunAndRotateForwardToWalkAndRotateBackwardAndRotation)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<RotateLeftEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateLeft});
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.backward});

    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<RotateRightEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.backward});

    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.backward});
    tiggerAndExpect<RotateLeftEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.backward});

    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<RotateRightEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.backward});

    tiggerAndExpect<EndRotationEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_IdleToWalkChangeDuringEquip)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, IdleArmedChangeState>({sut_.animationClipsNames_.equip},
                                                            {ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent, WalkArmedChangeState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<ArmedWalkState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.walk.forward});
}

TEST_F(CharacterControllerTests, Mixed_RunToWalkChangeDuringEquip)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<WeaponStateEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.armed.run.forward, sut_.animationClipsNames_.equip},
        {ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, WalkArmedChangeState>(
        {sut_.animationClipsNames_.armed.walk.forward, sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<ArmedWalkState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.walk.forward});
}

TEST_F(CharacterControllerTests, Mixed_ArmedRunToWalkChangeDuringDisarm)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<WeaponStateEvent, ArmedRunState>(
        {sut_.animationClipsNames_.armed.run.forward},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<WeaponStateEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.disarmed.run.forward, sut_.animationClipsNames_.disarm},
        {ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, WalkArmedChangeState>(
        {sut_.animationClipsNames_.disarmed.walk.forward, sut_.animationClipsNames_.disarm});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<DisarmedWalkState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.walk.forward});
}

TEST_F(CharacterControllerTests, Mixed_ArmedWalkToRunChangeDuringDisarm)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
    tiggerAndExpect<WeaponStateEvent, ArmedWalkState>(
        {sut_.animationClipsNames_.armed.walk.forward},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<WeaponStateEvent, WalkArmedChangeState>(
        {sut_.animationClipsNames_.disarmed.walk.forward, sut_.animationClipsNames_.disarm});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.disarmed.run.forward, sut_.animationClipsNames_.disarm});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmeRunBackwardIgnoreSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.backward});
    tiggerAndExpect<SprintStateChangeEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.backward});
    tiggerAndExpect<EndBackwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmeRunAndRotateBackwardIgnoreSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RotateLeftEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateLeft});
    tiggerAndExpect<MoveBackwardEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.backward});
    tiggerAndExpect<SprintStateChangeEvent, DisarmedRunAndRotateState>(
        {sut_.animationClipsNames_.disarmed.run.backward});
    tiggerAndExpect<EndRotationEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.backward});
    tiggerAndExpect<EndBackwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedWalkAndRotateBackwardIgnoreSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<RotateLeftEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateLeft});
    tiggerAndExpect<MoveBackwardEvent, DisarmedWalkAndRotateState>({sut_.animationClipsNames_.disarmed.walk.backward});
    tiggerAndExpect<SprintStateChangeEvent, DisarmedWalkAndRotateState>(
        {sut_.animationClipsNames_.disarmed.walk.backward});
    tiggerAndExpect<EndRotationEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmeWalkBackwardIgnoreSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});

    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.backward});
    tiggerAndExpect<SprintStateChangeEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmeIdleToSprint)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedSprintState>({sut_.animationClipsNames_.disarmed.sprint});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmeRotateToRotateAndSprint)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<RotateLeftEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateLeft});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedSprintAndRotateState>({sut_.animationClipsNames_.disarmed.sprint});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateLeft});
    tiggerAndExpect<EndRotationEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmeWalkToSprint)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedSprintState>({sut_.animationClipsNames_.disarmed.sprint});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmeRunToSprint)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedSprintState>({sut_.animationClipsNames_.disarmed.sprint});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedSprintState>({sut_.animationClipsNames_.disarmed.sprint});

    tiggerAndExpect<EndForwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmeWalkAndRotateToSprint)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});
    tiggerAndExpect<RotateRightEvent, DisarmedWalkAndRotateState>({sut_.animationClipsNames_.disarmed.walk.forward});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedSprintAndRotateState>({sut_.animationClipsNames_.disarmed.sprint});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedRunAndRotateState>(
        {sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarmed.rotateRight});
    tiggerAndExpect<EndRotationEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmeRunAndRotateToSprint)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, DisarmedRunState>({sut_.animationClipsNames_.disarmed.run.forward});
    tiggerAndExpect<RotateRightEvent, DisarmedRunAndRotateState>({sut_.animationClipsNames_.disarmed.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedSprintAndRotateState>({sut_.animationClipsNames_.disarmed.sprint});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedRunAndRotateState>(
        {sut_.animationClipsNames_.disarmed.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, DisarmedSprintAndRotateState>({sut_.animationClipsNames_.disarmed.sprint});
    tiggerAndExpect<EndRotationEvent, DisarmedSprintState>({sut_.animationClipsNames_.disarmed.sprint});
    tiggerAndExpect<EndForwardMoveEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedIdleToSprint)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintState>({sut_.animationClipsNames_.armed.sprint});
    tiggerAndExpect<EndForwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedRotateToRotateAndSprint)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<RotateLeftEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateLeft});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintAndRotateState>({sut_.animationClipsNames_.armed.sprint});
    tiggerAndExpect<EndForwardMoveEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateLeft});
    tiggerAndExpect<EndRotationEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedWalkToSprint)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintState>({sut_.animationClipsNames_.armed.sprint});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward});
    tiggerAndExpect<EndForwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmeRunToSprint)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintState>({sut_.animationClipsNames_.armed.sprint});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintState>({sut_.animationClipsNames_.armed.sprint});

    tiggerAndExpect<EndForwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmeWalkAndRotateToSprint)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<MoveForwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.forward});
    tiggerAndExpect<RotateRightEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.forward});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintAndRotateState>({sut_.animationClipsNames_.armed.sprint});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});
    tiggerAndExpect<EndForwardMoveEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateRight});
    tiggerAndExpect<EndRotationEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmeRunAndRotateToSprint)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.forward});
    tiggerAndExpect<RotateRightEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintAndRotateState>({sut_.animationClipsNames_.armed.sprint});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent, ArmedSprintAndRotateState>({sut_.animationClipsNames_.armed.sprint});
    tiggerAndExpect<EndRotationEvent, ArmedSprintState>({sut_.animationClipsNames_.armed.sprint});
    tiggerAndExpect<EndForwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

// TO do

TEST_F(CharacterControllerTests, Mixed_ArmeRunBackwardIgnoreSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.backward});
    tiggerAndExpect<SprintStateChangeEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.backward});
    tiggerAndExpect<EndBackwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmeRunAndRotateBackwardIgnoreSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RotateLeftEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateLeft});
    tiggerAndExpect<MoveBackwardEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.backward});
    tiggerAndExpect<SprintStateChangeEvent, ArmedRunAndRotateState>({sut_.animationClipsNames_.armed.run.backward});
    tiggerAndExpect<EndRotationEvent, ArmedRunState>({sut_.animationClipsNames_.armed.run.backward});
    tiggerAndExpect<EndBackwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedWalkAndRotateBackwardIgnoreSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<RotateLeftEvent, ArmedRotateState>({sut_.animationClipsNames_.armed.rotateLeft});
    tiggerAndExpect<MoveBackwardEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<SprintStateChangeEvent, ArmedWalkAndRotateState>({sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<EndRotationEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmeWalkBackwardIgnoreSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<MoveBackwardEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<SprintStateChangeEvent, ArmedWalkState>({sut_.animationClipsNames_.armed.walk.backward});
    tiggerAndExpect<EndBackwardMoveEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedWalkAndEquipWeapon)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectState<DisarmedIdleState>();
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<MoveForwardEvent, DisarmedWalkState>({sut_.animationClipsNames_.disarmed.walk.forward});

    tiggerAndExpect<WeaponStateEvent, ArmedWalkState>(
        {sut_.animationClipsNames_.armed.walk.forward},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<WeaponStateEvent, DisarmedWalkState>(
        {sut_.animationClipsNames_.disarmed.walk.forward},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}

TEST_F(CharacterControllerTests, Mixed_ArmdIdleToDrawStateAndBackToIdle)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});
    tiggerAndExpect<AimStopEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmdIdleToAimStateAndBackToIdle)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<AimState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.aimIdle});
    tiggerAndExpect<AimStopEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArrowSimpleShoot)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<AimState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.aimIdle});
    tiggerAndExpect<AttackEvent, DrawArrowState>(
        {sut_.animationClipsNames_.drawArrow},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<AimStopEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle});
}

TEST_F(CharacterControllerTests, Mixed_AimIdleToAimRotateLeft)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<AimState>();
    tiggerAndExpect<RotateLeftEvent, AimRotateState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.rotateLeft});
    tiggerAndExpect<RotateRightEvent, AimRotateState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.rotateRight});
    tiggerAndExpect<EndRotationEvent, AimState>({sut_.animationClipsNames_.aimIdle});
}

TEST_F(CharacterControllerTests, Mixed_DrawArrowToDisarmedState)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});
    tiggerAndExpect<WeaponStateEvent, DisarmedIdleState>(
        {sut_.animationClipsNames_.disarmed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}

TEST_F(CharacterControllerTests, Mixed_AimIdleToDisarmedState)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<AimState>();

    tiggerAndExpect<WeaponStateEvent, DisarmedIdleState>(
        {sut_.animationClipsNames_.disarmed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}

TEST_F(CharacterControllerTests, Mixed_ArrowSimpleShootToDisarmedState)
{
    expectState<DisarmedIdleState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.idle});
    tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent, DrawArrowState>({sut_.animationClipsNames_.drawArrow});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<AimState>();
    expectAnimsToBeSet({sut_.animationClipsNames_.aimIdle});
    tiggerAndExpect<AttackEvent, RecoilState>({sut_.animationClipsNames_.recoilArrow}, {ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<WeaponStateEvent, DisarmedIdleState>(
        {sut_.animationClipsNames_.disarmed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
