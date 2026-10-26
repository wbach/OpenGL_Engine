
#!/bin/bash

set -u

STATES_DIR="/mnt/nvme256/OpenGL_Engine/Sources/Tests/UT/Components/CharacterController/States"

# Sprawdź ścieżki przed rozpoczęciem
if [ ! -d "$STATES_DIR" ]; then
    echo "BŁĄD: Nie znaleziono katalogu: $STATES_DIR"
    exit 1
fi

for file in "$STATES_DIR"/*Tests.cpp; do
    [ -f "$file" ] || continue

    echo "=== Przetwarzanie pliku: $file ==="

    PROMPT_INSTRUCTION="
You are an automated C++20 code-refactoring agent.

Target test file: $file
Events:
MoveEvent
MoveForwardEvent
MoveBackwardEvent
MoveLeftEvent
MoveRightEvent
WalkChangeStateEvent
SprintStateChangeEvent
EndForwardMoveEvent
EndBackwardMoveEvent
EndMoveLeftEvent
EndMoveRightEvent
RotateLeftEvent
RotateRightEvent
RotateTargetEvent
EndRotationEvent
JumpEvent
JumpConfirmEvent
DodgeDiveEvent
DodgeForwardEvent
DodgeBackwardEvent
DodgeLeftEvent
DodgeRightEvent
DodgeEndEvent
AttackEvent
EndAttackEvent
DeathEvent
WeaponStateEvent
EquipEndStateEvent
DisarmEndStateEvent
DrawArrowEvent
ReloadArrowEvent
AimStartEvent
AimStopEvent
CrouchChangeStateEventSources/Tests/UT/Components/CharacterController/States/RecoilCrouchRotateStateTests.cpp
AnimationPlayEndEvent
ChangeAnimEvent
StartDialogEvent
EndDialogEvent

Instructions:
1. Read the target test file and identify the state name and existing test naming convention.
2. Verify that the file contains a test for every event, using the naming convention:
   <STATE_NAME>_<EVENT_NAME>
3. Add a test skeleton only for events whose tests are missing.
4. Follow the existing Google Test style and fixture naming convention in the file.
5. Use a TODO comment in each new test body.
6. Do not duplicate existing tests. Do not modify any other files.
7. Preserve all existing code and formatting as much as possible.
8. After editing, verify that the file contains the required tests and that no duplicates were introduced.
9. Return only a brief confirmation or an error message.
"

    if pi --print --no-session "$PROMPT_INSTRUCTION"; then
        echo "Zakończono: $file"
    else
        status=$?
        echo "BŁĄD: Agent zakończył pracę kodem $status dla pliku: $file"
    fi

    echo "-----------------------------------"
done

echo "Przetwarzanie zakończone."
