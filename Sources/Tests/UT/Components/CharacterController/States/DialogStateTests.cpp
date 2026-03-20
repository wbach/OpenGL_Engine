#include "../CharacterControllerTests.h"
#include "Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "Dialogs/Fsm/DialogEvents.h"

namespace
{
void prepareState(CharacterControllerTests& test)
{
    test.tiggerAndExpect({test.sut_.animationClipsNames_.disarmed.posture.stand.idle}, {ADVANCED_TIME_TRANSITION_TIME},
                         StartDialogEvent{.role = StartDialogEvent::Role::Waiting});
}
}  // namespace

TEST_F(CharacterControllerTests, DialogState_StartDialogListen)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    tiggerAndExpect({sut_.animationClipsNames_.disarmed.dialog.listen}, {ADVANCED_TIME_TRANSITION_TIME},
                    StartDialogEvent{.role = StartDialogEvent::Role::Listener});
}

TEST_F(CharacterControllerTests, DialogState_StartDialogSay)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});

    tiggerAndExpect({sut_.animationClipsNames_.disarmed.dialog.say}, {ADVANCED_TIME_TRANSITION_TIME},
                    StartDialogEvent{.role = StartDialogEvent::Role::Speaker});
}

TEST_F(CharacterControllerTests, DialogState_StartDialogUpdateToSay)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    prepareState(*this);

    tiggerAndExpect({sut_.animationClipsNames_.disarmed.dialog.say}, {ADVANCED_TIME_TRANSITION_TIME},
                    StartDialogEvent{.role = StartDialogEvent::Role::Speaker});
}

TEST_F(CharacterControllerTests, DialogState_End)
{
    expectAnimsToBeSet({sut_.animationClipsNames_.disarmed.posture.stand.idle});
    prepareState(*this);

    tiggerAndExpect({sut_.animationClipsNames_.disarmed.dialog.say}, {ADVANCED_TIME_TRANSITION_TIME},
                    StartDialogEvent{.role = StartDialogEvent::Role::Speaker});
    tiggerAndExpect({sut_.animationClipsNames_.disarmed.posture.stand.idle}, {ADVANCED_TIME_TRANSITION_TIME}, EndDialogEvent{});
}