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

TEST_F(CharacterControllerTests, DisarmedIdleState_RunBackwardEvent)
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
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_MoveBackwardEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_WalkChangeStateEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_SprintStateChangeEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_EndForwardMoveEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_EndBackwardMoveEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_EndMoveLeftEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_EndMoveRightEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_RotateTargetEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_EndRotationEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_JumpConfirmEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_DodgeDiveEvent)
{
    /* TODO: Implement test */
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
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_EndAttackEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_EquipEndStateEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_DisarmEndStateEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_ReloadArrowEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_AimStartEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_AimStopEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_StartFallingEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_GroundDetectionEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_HurtEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_AnimationPlayEndEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_ChangeAnimEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_StartDialogEvent)
{
    /* TODO: Implement test */
}

TEST_F(CharacterControllerTests, DisarmedIdleState_EndDialogEvent)
{
    /* TODO: Implement test */
}
