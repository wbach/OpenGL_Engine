#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.posture.stand.idle});
    test.expectRotationLeft();
    test.tiggerAndExpect<RotateLeftEvent>({test.sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
}
}  // namespace

TEST_F(CharacterControllerTests, DisarmedRotateState_DrawArrowEvent)
{
    prepareState(*this);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.left, sut_.animationClipsNames_.equip},
                                    {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_TRANSITION_TIME});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.rotate.left, sut_.animationClipsNames_.aim.draw});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_AimStopDuringDrawArrowEvent)
{
    prepareState(*this);
    expectRotationLeft(ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<DrawArrowEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.left, sut_.animationClipsNames_.equip},
                                    {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_TRANSITION_TIME});

    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    tiggerAndExpect<AimStopEvent>({sut_.animationClipsNames_.armed.posture.stand.rotate.left},
                                  {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_CLIP_TIME});

    // Extra check
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent>({sut_.animationClipsNames_.disarmed.posture.stand.death});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent>(
        {sut_.animationClipsNames_.armed.posture.stand.rotate.left, sut_.animationClipsNames_.equip});
    expectRotationLeft(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
    expectRotationLeft();
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectAnimsToBeSet({sut_.animationClipsNames_.armed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_RunForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveForwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_RUN_SPEED);
    tiggerAndExpect<MoveBackwardEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRotationLeft(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_BACKWARD);
}

TEST_F(CharacterControllerTests, DisarmedRotateState_MoveLeftEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_LEFT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveLeftEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRotationLeft(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_LEFT);
}

TEST_F(CharacterControllerTests, DisarmedRotateState_MoveRightEvent)
{
    prepareState(*this);
    expectVelocity(VECTOR_RIGHT, vec3(DEFAULT_RUN_SPEED, 0.0, 0));
    tiggerAndExpect<MoveRightEvent>({sut_.animationClipsNames_.disarmed.movement.run.forward});
    expectRotationLeft(DEFAULT_MOVING_CHANGE_DIR_SPEED);
    expectRootboneRotation(VECTOR_RIGHT);
}

TEST_F(CharacterControllerTests, DisarmedRotateState_RotateLeftEvent)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_RotateRightEvent)
{
    prepareState(*this);
    expectRotationRight();
    tiggerAndExpect<RotateRightEvent>({sut_.animationClipsNames_.disarmed.posture.stand.rotate.right});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_RotateTargetEvent)
{
    prepareState(*this);

    EXPECT_CALL(physicsApiMock_, SetRotation(rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));

    auto targetRotation = createRotaion(DEFAULT_TURN_SPEED, ADVANCED_TIME_TRANSITION_TIME);
    tiggerAndExpect<RotateTargetEvent>(RotateTargetEvent{targetRotation.value_},
                                       {sut_.animationClipsNames_.disarmed.posture.stand.rotate.left});
}

TEST_F(CharacterControllerTests, DisarmedRotateState_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndRotationEvent>({sut_.animationClipsNames_.disarmed.posture.stand.idle});
}

Quaternion caclulateTargetRotation(const vec3& toPlayer)
{
    auto angle2 = atan2f(toPlayer.x, toPlayer.z);
    Quaternion targertRotation;
    targertRotation.x = 0.f;
    targertRotation.y = 1.f * sinf(angle2 / 2.f);
    targertRotation.z = 0.f;
    targertRotation.w = cosf(angle2 / 2.f);

    return targertRotation;
}

TEST_F(CharacterControllerTests, DisarmedRotateState_XXX)
{
    //    context_.rotateToTargetProgress += (rotateSpeed_ * deltaTime);
    //    context_.rotateToTargetProgress = glm::clamp(context_.rotateToTargetProgress, 0.f, 1.f);

    //    auto newRotation =
    //        glm::slerp(context_.startRotation, context_.targetRotation, context_.rotateToTargetProgress);

    //    context_.rigidbody.SetRotation(newRotation);

    vec3 position(0);
    vec3 target(0, 0, -1);
    auto rotationRef = caclulateTargetRotation(target - position);

    for (int i = 0; i < 10; i++)
    {
        auto rotation = caclulateTargetRotation(target - position);

        auto d = glm::dot(rotation, rotationRef);
        /* LOG TO FIX*/  LOG_ERROR << (std::to_string(d));
        /* LOG TO FIX*/  LOG_ERROR << (std::to_string(ToDegrees(acosf(d))));

        target.x += 0.1f;
    }

    target = vec3(0, 0, -1);
    for (int i = 0; i < 10; i++)
    {
        auto rotation = caclulateTargetRotation(target - position);

        auto d = glm::dot(rotation, rotationRef);
        /* LOG TO FIX*/  LOG_ERROR << (std::to_string(d));
        /* LOG TO FIX*/  LOG_ERROR << (std::to_string(ToDegrees(acosf(d))));
        target.x -= 0.1f;
    }
}
