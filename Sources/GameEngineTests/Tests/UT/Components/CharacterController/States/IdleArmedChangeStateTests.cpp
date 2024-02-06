#include "../CharacterControllerTests.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    EXPECT_CALL(test.inputManagerMock_, CalcualteMouseMove()).WillRepeatedly(Return(vec2i{0, 0}));
    test.expectState<DisarmedIdleState>();
    test.expectAnimsToBeSet({test.sut_.animationClipsNames_.disarmed.idle});
    test.tiggerAndExpect<WeaponStateEvent, IdleArmedChangeState>({test.sut_.animationClipsNames_.equip});
}
}  // namespace

// TEST_F(CharacterControllerTests, IdleArmedChangeState_WeaponStateEvent)
//{
//     prepareState(*this);
//     tiggerAndExpect<WeaponStateEvent, IdleArmedChangeState>({sut_.animationClipsNames_.disarm},
//                                                             {ADVANCED_TIME_TRANSITION_TIME});
//     Update(ADVANCED_TIME_CLIP_TIME);
//     Update(ADVANCED_TIME_TRANSITION_TIME);
//     expectState<DisarmedIdleState>();
// }
TEST_F(CharacterControllerTests, IdleArmedChangeState_DeathEvent)
{
    prepareState(*this);
    tiggerAndExpect<DeathEvent, DeathState>({sut_.animationClipsNames_.disarmed.death},
                                            {ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, IdleArmedChangeState_EquipEndStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<EquipEndStateEvent, ArmedIdleState>({sut_.animationClipsNames_.armed.idle},
                                                        {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, IdleArmedChangeState_DisarmEndStateEvent)
{
    prepareState(*this);
    tiggerAndExpect<DisarmEndStateEvent, DisarmedIdleState>({sut_.animationClipsNames_.disarmed.idle},
                                                            {ADVANCED_TIME_CLIP_TIME, ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, IdleArmedChangeState_RunForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_RUN_SPEED);
    tiggerAndExpect<RunForwardEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.disarmed.run.forward},
        {ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, IdleArmedChangeState_RunBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_RUN_SPEED);
    tiggerAndExpect<RunBackwardEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.disarmed.run.backward},
        {ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, IdleArmedChangeState_WalkForwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(DEFAULT_WALK_SPEED);
    tiggerAndExpect<WalkForwardEvent, WalkArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.disarmed.walk.forward},
        {ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, IdleArmedChangeState_WalkBackwardEvent)
{
    prepareState(*this);
    expectForwardVelocity(-DEFAULT_BACKWARD_WALK_SPEED);
    tiggerAndExpect<WalkBackwardEvent, WalkArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.disarmed.walk.backward},
        {ADVANCED_TIME_TRANSITION_TIME});
}
TEST_F(CharacterControllerTests, IdleArmedChangeState_SprintStartEvent)
{
    prepareState(*this);
    tiggerAndExpect<SprintStartEvent, RunArmedChangeState>(
        {sut_.animationClipsNames_.equip, sut_.animationClipsNames_.disarmed.run.forward},
        {ADVANCED_TIME_TRANSITION_TIME});

    expectForwardVelocity(DEFAULT_SPRINT_SPEED);
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);
    expectState<ArmedSprintState>();
}
