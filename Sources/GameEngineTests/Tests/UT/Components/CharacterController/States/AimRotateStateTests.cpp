#include "../CharacterControllerTests.h"

// struct WalkForwardEvent;
// struct WalkBackwardEvent;
// struct WalkChangeStateEvent;
// struct RunForwardEvent;
// struct RunBackwardEvent;
// struct MoveLeftEvent;
// struct MoveRightEvent;
// struct RunEvent;
// struct CrouchEvent;
// struct SprintStartEvent;
// struct SprintStateChangeEvent;
// struct EndForwardMoveEvent;
// struct EndBackwardMoveEvent;
// struct RotateLeftEvent;
// struct RotateRightEvent;
// struct RotateTargetEvent;
// struct EndRotationEvent;
// struct JumpEvent;
// struct EndJumpEvent;
// struct AttackEvent;
// struct NextAttackEvent;
// struct EndAttackEvent;
// struct DeathEvent;
// struct WeaponStateEvent;
// struct WeaponChangeEndEvent;
// struct DrawArrowEvent;
// struct AimStartEvent;
// struct AimStopEvent;
// struct ShootEvent;

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.physicsApiMock_, SetRotation(test.rigidbodyid, Matcher<const Quaternion&>(_))).Times(AtLeast(1));
    EXPECT_CALL(test.physicsApiMock_, GetRotation(test.rigidbodyid)).WillRepeatedly(Return(Rotation().value_));

    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent, ArmedIdleState>(
        {test.sut_.animationClipsNames_.armed.idle},
        {ADVANCED_TIME_TRANSITION_TIME, ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});

    test.tiggerAndExpect<DrawArrowEvent, DrawArrowState>({test.sut_.animationClipsNames_.drawArrow});
    test.Update(ADVANCED_TIME_CLIP_TIME);
    test.Update(ADVANCED_TIME_TRANSITION_TIME);
    test.expectState<AimState>();
    test.tiggerAndExpect<RotateLeftEvent, AimRotateState>(
        {test.sut_.animationClipsNames_.aimIdle, test.sut_.animationClipsNames_.armed.rotateLeft});
}
}  // namespace

TEST_F(CharacterControllerTests, AimRotate_RotateLeft)
{
    prepareState(*this);
    tiggerAndExpect<RotateLeftEvent, AimRotateState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.rotateLeft});
}
TEST_F(CharacterControllerTests, AimRotate_RotateRight)
{
    prepareState(*this);
    tiggerAndExpect<RotateRightEvent, AimRotateState>(
        {sut_.animationClipsNames_.aimIdle, sut_.animationClipsNames_.armed.rotateRight});
}
TEST_F(CharacterControllerTests, AimRotate_WeaponStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<WeaponStateEvent, DisarmedRotateState>({sut_.animationClipsNames_.disarm, sut_.animationClipsNames_.disarmed.rotateLeft});
}
TEST_F(CharacterControllerTests, AimRotate_AttackEvent)
{
    prepareState(*this);
    tiggerAndExpect<AttackEvent, RecoilRotateState>({ sut_.animationClipsNames_.armed.rotateLeft, sut_.animationClipsNames_.recoilArrow });
}
TEST_F(CharacterControllerTests, AimRotate_EndRotationEvent)
{
    prepareState(*this);
    tiggerAndExpect<EndRotationEvent, AimState>({ sut_.animationClipsNames_.aimIdle });
}
TEST_F(CharacterControllerTests, AimRotate_AimStopEvent)
{
    prepareState(*this);
    tiggerAndExpect<AimStopEvent, ArmedRotateState>({ sut_.animationClipsNames_.armed.rotateLeft });
}
