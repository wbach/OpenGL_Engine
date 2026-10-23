#include "../CharacterControllerTests.h"
#include "Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "Components/Controllers/CharacterController/States/DisarmedIdleState.h"
#include "GameEngine/Physics/CollisionContactInfo.h"

TEST_F(CharacterControllerTests, DisarmedIdleState_DrawArrowEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.equip.front()}, {ADVANCED_TIME_TRANSITION_TIME});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.aim.draw});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_AimStopDuringDrawArrowEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.equip.front()}, {ADVANCED_TIME_TRANSITION_TIME});

    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.equip.front()});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.idle});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_WeaponStateEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.equip.front()}, {ADVANCED_TIME_TRANSITION_TIME});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
}

TEST_F(CharacterControllerTests, DisarmedIdleState_BlockWeaponStateEventWithoutAnim)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    sut_.animationClipsNames_.equip.clear();
    tiggerAndExpect<WeaponStateEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle}, {ADVANCED_TIME_TRANSITION_TIME});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    EXPECT_EQ(sut_.getCurrentStateName(), typeName<DisarmedIdleState>());
}

TEST_F(CharacterControllerTests, DisarmedIdleState_DeathEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.posture.stand.death});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_MoveForwardEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRootboneRotation(VECTOR_FORWARD);
}

TEST_F(CharacterControllerTests, DisarmedIdleState_MoveBackwardEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, DisarmedIdleState_MoveLeftEvent)
{
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRootboneRotation(VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, DisarmedIdleState_MoveRightEvent)
{
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRootboneRotation(VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, DisarmedIdleState_RotateLeftEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectRotationLeft();
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_RotateRightEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.right});
}

TEST_F(CharacterControllerTests, DISABLED_DisarmedIdleState_RotateTargetEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.disarmed.posture.stand.rotate.right});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_AttackEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.attack.front().clipsSequence.front()},
                                 {ADVANCED_TIME_TRANSITION_TIME});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_BlockTranistionAfterAttackEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    sut_.animationClipsNames_.disarmed.attack.clear();
    tiggerAndExpect<AttackEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle}, {ADVANCED_TIME_TRANSITION_TIME});

    EXPECT_EQ(sut_.getCurrentStateName(), typeName<DisarmedIdleState>());
}

TEST_F(CharacterControllerTests, DisarmedIdleState_CrouchChangeStateEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<CrouchChangeStateEvent>({sut_.animationClipsNames_.disarmed.posture.crouched.idle},
                                            {ADVANCED_TIME_TRANSITION_TIME});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_JumpEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    Physics::CollisionContactInfos infos;
    infos.push_back({});

    for (int i = 0; i < 3; i++)
    {
        /* LOG TO FIX*/ LOG_ERROR << ("Iteration i=" + std::to_string(i));
        EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).WillRepeatedly(Return(vec3(0)));
        EXPECT_CALL(physicsApiMock_, SetVelocityRigidbody(rigidbodyid, vec3(0.f, DEFAULT_JUMP_POWER, 0.f)));

        handleEvent(JumpEvent{DEFAULT_JUMP_POWER});

        EXPECT_CALL(physicsApiMock_, GetVelocity(rigidbodyid)).Times(0);
        handleEvent(JumpEvent{DEFAULT_JUMP_POWER});

        infos.clear();
        groundExitSub.second.callback({});

        Update(ADVANCED_TIME_TRANSITION_TIME);
        expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.jump});

        auto rId = rigidbody_->GetId().value();
        std::vector<Physics::CollisionContactInfo> groundDetectinfos{
            {.pos1         = vec3(0),
             .pos2         = rigidbody_->GetParentGameObject().GetWorldTransform().GetPosition(),
             .rigidbodyId1 = rId,
             .rigidbodyId2 = rId + 10}};

        groundEnterSub.second.callback(groundDetectinfos);
        infos.push_back({});
        //  monitoringCollisionsCallback(infos);
        Update(ADVANCED_TIME_TRANSITION_TIME);

        expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    }
}

TEST_F(CharacterControllerTests, DisarmedIdleState_MoveEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRootboneRotation(VECTOR_FORWARD);
}

TEST_F(CharacterControllerTests, DisarmedIdleState_WalkChangeStateEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectIngoreEvent<WalkChangeStateEvent>();
}

TEST_F(CharacterControllerTests, DisarmedIdleState_SprintStateChangeEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectIngoreEvent<SprintStateChangeEvent>();
}

TEST_F(CharacterControllerTests, DisarmedIdleState_EndForwardMoveEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectIngoreEvent<EndForwardMoveEvent>();
}

TEST_F(CharacterControllerTests, DisarmedIdleState_EndBackwardMoveEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectIngoreEvent<EndBackwardMoveEvent>();
}

TEST_F(CharacterControllerTests, DisarmedIdleState_EndMoveLeftEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectIngoreEvent<EndMoveLeftEvent>();
}

TEST_F(CharacterControllerTests, DisarmedIdleState_EndMoveRightEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectIngoreEvent<EndMoveRightEvent>();
}

TEST_F(CharacterControllerTests, DisarmedIdleState_RotateTargetEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_EndRotationEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectIngoreEvent<EndRotationEvent>();
}

TEST_F(CharacterControllerTests, DisarmedIdleState_JumpConfirmEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectIngoreEvent<JumpConfirmEvent>();
}

TEST_F(CharacterControllerTests, DisarmedIdleState_DodgeDiveEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<DodgeDiveEvent>({sut_.animationClipsNames_.disarmed.dodgeDive});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_DodgeForwardEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_DodgeBackwardEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_DodgeLeftEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_DodgeRightEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_DodgeEndEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectIngoreEvent<DodgeEndEvent>();
}

TEST_F(CharacterControllerTests, DisarmedIdleState_EndAttackEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectIngoreEvent<EndAttackEvent>();
}

TEST_F(CharacterControllerTests, DisarmedIdleState_EquipEndStateEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectIngoreEvent<EquipEndStateEvent>();
}

TEST_F(CharacterControllerTests, DisarmedIdleState_DisarmEndStateEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectIngoreEvent<DisarmEndStateEvent>();
}

TEST_F(CharacterControllerTests, DisarmedIdleState_ReloadArrowEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<ReloadArrowEvent>({sut_.animationClipsNames_.equip.front()}, {ADVANCED_TIME_TRANSITION_TIME});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.aim.draw});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_AimStartEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    // First, the character must draw the arrow
    tiggerAndExpect<AimStartEvent>({sut_.animationClipsNames_.equip.front()}, {ADVANCED_TIME_TRANSITION_TIME});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.aim.draw});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.aim.idle});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_AimStopEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectIngoreEvent<AimStopEvent>();
}

TEST_F(CharacterControllerTests, DisarmedIdleState_StartFallingEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<StartFallingEvent>({sut_.animationClipsNames_.disarmed.falling});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_GroundDetectionEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectIngoreEvent<GroundDetectionEvent>();
}

TEST_F(CharacterControllerTests, DisarmedIdleState_HurtEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect<HurtEvent>({sut_.animationClipsNames_.disarmed.posture.stand.hurt});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_AnimationPlayEndEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectIngoreEvent<AnimationPlayEndEvent>();
}

TEST_F(CharacterControllerTests, DisarmedIdleState_ChangeAnimEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectIngoreEvent<ChangeAnimEvent>();
}

TEST_F(CharacterControllerTests, DisarmedIdleState_StartDialogEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    tiggerAndExpect({sut_.animationClipsNames_.disarmed.dialog.listen}, {ADVANCED_TIME_TRANSITION_TIME},
                    StartDialogEvent{.role = StartDialogEvent::Role::Listener});
}

TEST_F(CharacterControllerTests, DisarmedIdleState_EndDialogEvent)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    expectIngoreEvent<EndDialogEvent>();
}
