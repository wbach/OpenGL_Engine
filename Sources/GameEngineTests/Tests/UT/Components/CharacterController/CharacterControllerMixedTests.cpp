#include "CharacterControllerTests.h"

TEST_F(CharacterControllerTests, Mixed_DisarmedState_IdleToRunForward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    handleEvent(MoveForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});

    handleEvent(EndForwardMoveEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedState_IdleToRunBackward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, -DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    handleEvent(MoveBackwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectRootboneRotation(VECTOR_BACKWARD);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});

    handleEvent(EndBackwardMoveEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedState_IdleToRunForwardAndBackward)
{
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    handleEvent(MoveForwardEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});

    expectNoMove();
    handleEvent(MoveBackwardEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    handleEvent(EndBackwardMoveEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedToArmedStateIdle)
{
    handleEvent(WeaponStateEvent{});

    // Animation transition : None -> Armed idle state
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.equip});

    // Animation : EquipAnim
    Update(ADVANCED_TIME_CLIP_TIME);

    // State need to change
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_RunForwardDuringDisarmedToArmedState)
{
    handleEvent(WeaponStateEvent{});

    // Animation transition : None -> Armed idle state (
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.equip});

    // Animation : EquipAnim
    Update(DUMMY_CLIP_LENGTH / 2.f);

    handleEvent(MoveForwardEvent{});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip});

    Update(DUMMY_CLIP_LENGTH / 2.f);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward});
}

TEST_F(CharacterControllerTests, Mixed_EquipWeaponDuringDisarmedRunForward)
{
    vec3 runningVelocity(0.0, 0.0, DEFAULT_RUN_SPEED);

    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, runningVelocity)).Times(AtLeast(1));

    handleEvent(MoveForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});

    handleEvent(WeaponStateEvent{});

    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(runningVelocity));

    // PlayMixedAnimation with equip
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip});

    // Animation : Tirgger EquipAnim end, and notify state
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward});
    Update(ADVANCED_TIME_TRANSITION_TIME);
}

TEST_F(CharacterControllerTests, Mixed_DisarmWeaponDuringArmedRunForward)
{
    vec3 runningVelocity(0.0, 0.0, DEFAULT_RUN_SPEED);

    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, runningVelocity)).Times(AtLeast(1));

    handleEvent(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);

    handleEvent(MoveForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward});

    handleEvent(WeaponStateEvent{});

    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(runningVelocity));

    // PlayMixedAnimation with equip
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward, sut_.animationClipsNames_.disarm});

    // Animation : Tirgger EquipAnim end, and notify state
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});
    Update(ADVANCED_TIME_TRANSITION_TIME);
}

TEST_F(CharacterControllerTests, Mixed_EquipWeaponDuringRotateLeftState)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    handleEvent(RotateLeftEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});

    handleEvent(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.rotate.left, sut_.animationClipsNames_.equip});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.rotate.left});

    handleEvent(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left, sut_.animationClipsNames_.disarm});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});

    handleEvent(EndRotationEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_EquipWeaponDuringRotateRightState)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    handleEvent(RotateRightEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.rotate.right});

    handleEvent(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.rotate.right, sut_.animationClipsNames_.equip});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.rotate.right});

    handleEvent(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.rotate.right, sut_.animationClipsNames_.disarm});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.rotate.right});

    handleEvent(EndRotationEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedStateToArmed_RunForwardAndRotateLeft)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    handleEvent(MoveForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});

    handleEvent(RotateLeftEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});

    handleEvent(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward});

    handleEvent(EndRotationEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward});

    handleEvent(EndForwardMoveEvent{});
    ;

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedStateToArmed_RunForwardAndRotateRight)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    handleEvent(MoveForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});

    handleEvent(RotateRightEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});

    handleEvent(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward});

    handleEvent(EndRotationEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward});

    handleEvent(EndForwardMoveEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedState_RunForwardAndRotateLeftAndChangeWeaponState)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    handleEvent(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.equip});

    handleEvent(MoveForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip});

    handleEvent(RotateLeftEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip});

    handleEvent(WeaponStateEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward, sut_.animationClipsNames_.disarm});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});

    handleEvent(EndForwardMoveEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});

    handleEvent(EndRotationEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}
TEST_F(CharacterControllerTests, Mixed_DisamredToArmedState_DuringEquipRunForwardAndRotateLeftAndChangeWeaponState)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    handleEvent(WeaponStateEvent{});
    handleEvent(MoveForwardEvent{});

    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedStopMovingDuringEuip)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.0, 0.0, DEFAULT_RUN_SPEED)))
        .Times(AtLeast(1));

    handleEvent(MoveForwardEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});
    handleEvent(WeaponStateEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    handleEvent(EndForwardMoveEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.equip});
    handleEvent(WeaponStateEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisamredDuringEquip)
{
    handleEvent(WeaponStateEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.equip});
    handleEvent(WeaponStateEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarm});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisamredDuringEquipTransition)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    handleEvent(WeaponStateEvent{});

    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.disarm});
    Update(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_IdleToWalkToIdleState)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_IdleChangeWalkEventDoNothing)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_RunToWalkToRunState)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_WalkAndRotateState)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.right});
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_RotateAndWalkState)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.right});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.right});
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_RunAndRotateToWalkAndRotate)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.right});
}

TEST_F(CharacterControllerTests, Mixed_RunAndRotateToWalkAndRotateToWalkForward)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedIdleToWalkForwardBackward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedIdleToWalkBackwardForward)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedIdleToWalkForward)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedIdleToWalkBackward)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);

    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
    expectRootboneRotation(VECTOR_BACKWARD);

    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, Mixed_ArmedIdleToWalkForwardBackward)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
    expectNoMove();
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedIdleDoNothingIfWalkChangeEvent)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedRunToWalkForwardToRun)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedRunToWalkBackwardToRun)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, Mixed_ArmedRunAndRotateForwardToWalkAndRotateForwardAndRotation)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.left});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.armed.movement.run.forward});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});

    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedRunAndRotateForwardToWalkAndRotateBackwardAndRotation)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.left});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward});

    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});

    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.armed.movement.run.forward});

    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});

    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_IdleToWalkChangeDuringEquip)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.equip}, {ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip},
                                      {0});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>(
        {sut_.animationClipsNames_.armed.movement.walk.forward, sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.walk.forward});
}

TEST_F(CharacterControllerTests, Mixed_RunToWalkChangeDuringEquip)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.armed.movement.run.forward, sut_.animationClipsNames_.equip},
                                      {ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>(
        {sut_.animationClipsNames_.armed.movement.walk.forward, sut_.animationClipsNames_.equip});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.movement.walk.forward});
}

TEST_F(CharacterControllerTests, Mixed_ArmedRunToWalkChangeDuringDisarm)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.movement.run.forward},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarmed.movement.run.forward, sut_.animationClipsNames_.disarm},
        {ADVANCED_TIME_TRANSITION_TIME});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>(
        {sut_.animationClipsNames_.disarmed.movement.walk.forward, sut_.animationClipsNames_.disarm});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.walk.forward});
}

TEST_F(CharacterControllerTests, Mixed_ArmedWalkToRunChangeDuringDisarm)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.movement.walk.forward},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarmed.movement.walk.forward, sut_.animationClipsNames_.disarm});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>(
        {sut_.animationClipsNames_.disarmed.movement.run.forward, sut_.animationClipsNames_.disarm});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.movement.run.forward});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmeRunBackwardToSprint)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
    expectForwardVelocity(-DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.sprint});
    expectRootboneRotation(VECTOR_BACKWARD);
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmeRunAndRotateBackwardSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectForwardVelocity(-DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.sprint});
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.disarmed.sprint});
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedWalkAndRotateBackwardSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
    expectForwardVelocity(-DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.sprint});
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.disarmed.sprint});
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmeWalkBackwardSprintEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
    expectForwardVelocity(-DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.sprint});
    expectRootboneRotation(VECTOR_BACKWARD);
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmeIdleToSprint)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.sprint});
    expectRootboneRotation(VECTOR_FORWARD);
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmeRotateToRotateAndSprint)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.sprint});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmeWalkToSprint)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.sprint});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmeRunToSprint)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.sprint});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.sprint});

    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmeWalkAndRotateToSprint)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.sprint});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.right});
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmeRunAndRotateToSprint)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.sprint});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.disarmed.sprint});
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.disarmed.sprint});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedIdleToSprint)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward}, {0});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedRotateToRotateAndSprint)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.left});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward}, {0});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.left});
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedWalkToSprint)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmeRunToSprint)
{
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.movement.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});

    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmeWalkAndRotateToSprint)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.right});
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmeRunAndRotateToSprint)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.armed.movement.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});

    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.movement.run.forward});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.armed.sprint});
    tiggerAndExpect<EndForwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmeRunBackwardSprintEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    expectForwardVelocity(-DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmeRunAndRotateBackwardSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.left});
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward});
    expectForwardVelocity(-DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.armed.sprint});
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmedWalkAndRotateBackwardSprintEvent)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));
    EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.left});
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward}, {0});
    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
    expectForwardVelocity(-DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.armed.sprint});
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmeWalkBackwardSprintEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.armed.movement.run.forward}, {0});
    expectForwardVelocity(-DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.armed.movement.walk.forward});
    expectForwardVelocity(-DEFAULT_SPRINT_SPEED);
    tiggerAndExpect<SprintStateChangeEvent>({sut_.animationClipsNames_.armed.sprint});
    tiggerAndExpect<EndBackwardMoveEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_DisarmedWalkAndEquipWeapon)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward}, {0});
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkChangeStateEvent>({sut_.animationClipsNames_.disarmed.movement.walk.forward});

    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.movement.walk.forward},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarmed.movement.walk.forward},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}

TEST_F(CharacterControllerTests, Mixed_ArmdIdleToDrawStateAndBackToIdle)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.aim.draw});
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArmdIdleToAimStateAndBackToIdle)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.aim.draw});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.aim.idle});
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_ArrowSimpleShoot)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.aim.draw});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.aim.idle});
    tiggerAndExpect<AttackEvent>(
        {sut_.animationClipsNames_.aim.draw},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, Mixed_AimIdleToAimRotateLeft)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.aim.draw});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.aim.idle, sut_.animationClipsNames_.armed.posture.stand.rotate.left});
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.aim.idle, sut_.animationClipsNames_.armed.posture.stand.rotate.right});
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.aim.idle});
}

TEST_F(CharacterControllerTests, Mixed_DrawArrowToDisarmedState)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.aim.draw});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarmed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}

TEST_F(CharacterControllerTests, Mixed_AimIdleToDisarmedState)
{
    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(0));
    EXPECT_CALL(physicsApiMock_, GetRotation(rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.aim.draw});

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarmed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}

TEST_F(CharacterControllerTests, Mixed_ArrowSimpleShootToDisarmedState)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.aim.draw});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({sut_.animationClipsNames_.aim.idle});
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.aim.recoil}, {ADVANCED_TIME_TRANSITION_TIME});
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.disarmed.posture.stand.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
