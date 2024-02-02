#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, GetVelocity(test.rigidbodyid)).WillRepeatedly(Return(vec3(0)));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.expectRotationRight(ADVANCED_TIME_TRANSITION_TIME);
    test.tiggerAndExpect<RotateRightEvent, ArmedRotateState>({test.sut_.animationClipsNames_.armed.rotateRight});
}
}  // namespace

TEST_F(CharacterControllerTests, ArmedRotate_DrawArrowEvent)
{
    prepareState(*this);
    tiggerAndExpect<DrawArrowEvent, DrawArrowRotateState>(
        {sut_.animationClipsNames_.drawArrow, sut_.animationClipsNames_.armed.rotateRight});
}


//Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
//Utils::StateMachine::On<SprintStartEvent, Utils::StateMachine::TransitionTo<ArmedSprintAndRotateState>>,
//Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::TransitionTo<ArmedSprintAndRotateState>>,
//Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<ArmedSprintAndRotateState>>,

////struct SprintStateChangeEvent
////{
////};
//Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
//Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
//Utils::StateMachine::On<WeaponChangeEndEvent, Utils::StateMachine::Update>,
//Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::Update>,
//Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::TransitionTo<ArmedWalkAndRotateState>>,
//Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::TransitionTo<ArmedWalkAndRotateState>>,
//Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedRotateState>>,
//Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
//Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
//Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
//Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
//Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
//Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
//Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowRotateState>>,

//Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
