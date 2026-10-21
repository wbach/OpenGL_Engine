---
name: character-controller-test-debugger
description: Diagnose and fix failing C++ CharacterControllerTests in the OpenGL Engine. Locate tests across state-specific and mixed-scenario files, trace events and state transitions, analyze animation, movement, rotation, timing, and Google Mock failures, then verify fixes with targeted tests and regression checks.
---

# Character Controller Test Debugger

## 1. Mission

Your task is to investigate, diagnose, and fix failing `CharacterControllerTests` in the OpenGL Engine project.

These tests verify character behavior in response to gameplay events, including:

- State transitions and event handling.
- Animation selection and animation transitions.
- Character movement and velocity.
- Character rotation.
- Timing-dependent behavior, including animation completion and state transitions.
- Interactions between different character states.

Your primary responsibility is to identify the underlying cause of a failure, implement the smallest correct fix, and verify that the fix does not introduce regressions.

Do not blindly change code to make an assertion pass. Understand the intended scenario and actual execution flow first.

## 2. Project environment and test execution

The test build directory is:

`/mnt/nvme256/OpenGL_Engine/build/ubuntu`

Run an individual failing test using its fully qualified Google Test name:

```bash
cd /mnt/nvme256/OpenGL_Engine/build/ubuntu

./GameEngineTestsExe --verbose \
  --gtest_filter="CharacterControllerTests.Mixed_ArmedChangeDuringDiveDoge"
```

Replace the example test name with the actual failing test.

Rebuild after modifying production code or test code:

```bash
cd /mnt/nvme256/OpenGL_Engine/build/ubuntu
ninja
```

Then rerun the targeted test.

If the build fails, investigate the compilation error before attempting to interpret the test failure.

If the targeted test passes, run relevant neighboring tests. If the change affects shared state logic, event dispatch, animation handling, or movement, run a broader set of tests when practical.

Always report the actual build and test results. Never claim verification that was not performed.

## 3. Test organization and file discovery

The `CharacterControllerTests` suite is split across multiple source files because of the large number of scenarios.

The test files are organized by the states or state interactions they cover:

- State-specific tests are grouped into files associated with individual states, including files under the `states/` subdirectory.
- Mixed-scenario tests cover interactions between multiple states or transitions between states.
- Mixed-scenario tests use the `Mixed_` prefix in their Google Test names.

Use the test name to locate the test definition before investigating the implementation.

For example:

```bash
grep -R -n 'Mixed_ArmedChangeDuringDiveDoge' \
  /mnt/nvme256/OpenGL_Engine \
  --include='*.cpp' \
  --include='*.hpp' \
  --include='*.h'
```

Prefer searching the relevant test directories first if a repository-wide search is unnecessarily expensive.

After locating the test:

1. Read the complete test body.
2. Identify the source file and the state or states being tested.
3. Inspect the test fixture, setup, teardown, and relevant helper methods.
4. Locate the implementation of the states involved.
5. Trace the event-handling and transition logic connecting those states.
6. Search for related tests that exercise similar events or transitions.

Do not assume that a test belongs exclusively to the state mentioned first in its name. Verify the complete scenario.

### Interpreting the `Mixed_` prefix

The `Mixed_` prefix indicates a scenario that combines multiple states or behaviors, often to test interactions and transitions.

For example:

`CharacterControllerTests.Mixed_ArmedChangeDuringDiveDoge`

This test combines a dodge-dive scenario with a weapon-state change.

Investigate how the weapon-state event is handled while the character is performing or transitioning from the dodge-dive behavior. Follow the complete sequence through subsequent animation and movement updates.

For a mixed test, do not stop investigating after finding a plausible bug in one state. Verify how the involved states interact.

Test names and file organization are useful debugging clues, not proof of the actual runtime state or root cause.

## 4. First diagnose the failure

Start by reading the complete test output.

Identify the first meaningful failure and record:

- The failed assertion or unexpected mock invocation.
- The expected behavior or arguments.
- The actual behavior, if reported.
- The source file and line number.
- Whether the failure is an assertion failure, mock expectation failure, compilation error, crash, or another runtime problem.

For Google Mock failures, inspect:

- Expected versus actual call counts.
- Unexpected method invocations.
- Argument mismatches.
- Missing expected calls.
- The lifetime and applicability of expectations.
- Whether multiple expectations on the same method affect which calls are accepted.

The first meaningful failure is usually the best starting point. Later failures may be consequences of an earlier incorrect transition.

Do not independently patch every failed assertion before determining whether the failures share one root cause.

## 5. Reconstruct the scenario from logs

Reconstruct the actual runtime flow chronologically.

Start at the point where the scenario begins after test setup and initialization. Find the first gameplay event, often the event that causes the character to leave its initial idle behavior.

Follow the logs from that event through the rest of the scenario until its completion and cleanup, where applicable.

For each significant step, determine:

1. Which event was dispatched?
2. Which state handled the event?
3. What was the state before the event?
4. Was a transition requested?
5. Which state became active afterward?
6. Which animation was selected?
7. What movement or rotation behavior was expected?
8. What actually happened, according to the available evidence?
9. How much simulated time elapsed?
10. Did the state, animation, or movement change unexpectedly?

Build a short chronological timeline when the scenario is complex.

Example:

| Step | Action | Expected behavior |
|---|---|---|
| 1 | `MoveForwardEvent` | Character begins forward movement |
| 2 | Advance transition time | Forward-running animation is set |
| 3 | `DodgeDiveEvent` | Character enters dodge-dive behavior |
| 4 | Advance transition time | Dodge-dive animation is set |
| 5 | `WeaponStateEvent` | Weapon-state change is processed during the current scenario |
| 6 | Advance clip and transition times | Expected equip and armed movement animations are set |

This table represents the intended test sequence, not proof that the runtime followed it.

If logs do not expose the actual state or animation, inspect the relevant implementation. Add temporary, targeted diagnostics only when necessary and remove them afterward if they are not intended to remain.

Do not infer a successful transition merely because the corresponding event appears in the logs.

## 6. Understand the test helpers

### 6.1 `handleEvent(event)`

Example:

```cpp
handleEvent(MoveForwardEvent{});
```

This dispatches an event into the character-controller test setup.

An event can cause a state transition or change the current state's behavior.

When investigating an event, determine:

- Which state receives it.
- How that state handles it.
- Whether the event is ignored, consumed, deferred, or handled.
- Whether it requests a transition.
- Whether the transition happens immediately or depends on an update or animation completion.
- What behavior should follow the event.

Do not assume that dispatching an event guarantees that the intended state has already become active.

### 6.2 `Update(time)`

Example:

```cpp
Update(ADVANCED_TIME_TRANSITION_TIME);
```

This advances simulated time and executes the relevant component update logic.

Tests use it to allow time-dependent behavior to progress, including:

- Animation transitions.
- Animation clip completion.
- State transitions triggered by elapsed time.
- Movement and rotation updates.
- Other update-driven state behavior.

The exact duration and order of updates matter.

When debugging, preserve the original sequence in your mental model and determine what each update enables. Do not remove, reorder, or combine updates without understanding their effects.

### 6.3 `expectAnimsToBeSet(clipNames)`

Example:

```cpp
expectAnimsToBeSet({
    sut_.animationClipsNames_.disarmed.movement.run.forward
});
```

This verifies that the expected animation clip or clips are set at that point in the scenario.

When this expectation fails:

1. Record the expected clips.
2. Determine the actual selected clips, if observable.
3. Identify the state responsible for selecting them.
4. Trace the preceding event and update calls.
5. Check whether the failure comes from event handling, state transition, animation selection, or timing.

Do not replace the expected animation with the observed animation merely to make the test pass. Establish the intended behavior first.

### 6.4 `EXPECT_CALL` and movement behavior

Example:

```cpp
EXPECT_CALL(
    physicsApiMock_,
    SetVelocityRigidbody(
        rigidbodyid,
        vec3(0.0, 0.0, DEFAULT_RUN_SPEED)
    )
).Times(AtLeast(1));
```

This requires at least one matching invocation of `SetVelocityRigidbody` during the effective lifetime of the expectation.

It does not, by itself, guarantee that every subsequent call uses the same velocity or that the character maintains that velocity until the next assertion.

When investigating movement, determine:

- Whether the expected method was called.
- The actual arguments passed to each relevant invocation.
- Whether the velocity has the correct direction and magnitude.
- Whether a later update recalculates or resets velocity.
- Whether a state transition is expected to preserve, modify, or stop movement.
- Whether later mock expectations change which calls are accepted.

If the test is intended to verify sustained movement, determine how that behavior is asserted. Do not infer it solely from `Times(AtLeast(1))`.

Pay particular attention to zero velocity, unexpected direction changes, and movement that is reset during state transitions.

### 6.5 Rotation expectations

When a test mocks `GetRotation`, `SetRotation`, or related physics methods, determine:

- Whether the current state should preserve the existing rotation.
- Whether the movement direction should change the character's rotation.
- Whether rotation is updated immediately or during `Update`.
- Whether the expected rotation uses the correct representation and coordinate system.
- Whether a transition changes the rotation policy.

Do not modify rotation logic without checking the intended behavior of the current state and related states.

### 6.6 `tiggerAndExpect`

The test suite contains a helper similar to:

```cpp
template <typename Event>
void tiggerAndExpect(
    const Event& event,
    const std::vector<std::string>& clipNames,
    const std::vector<float>& updateTimes = {
        ADVANCED_TIME_TRANSITION_TIME
    })
{
    handleEvent(event);

    for (auto updateTime : updateTimes)
        Update(updateTime);

    expectAnimsToBeSet(clipNames);
}
```

The helper combines event dispatch, time advancement, and animation verification.

For debugging, mentally expand it into individual operations.

For example:

```cpp
tiggerAndExpect(
    MoveForwardEvent{},
    {sut_.animationClipsNames_.disarmed.movement.run.forward}
);
```

Conceptually becomes:

```cpp
handleEvent(MoveForwardEvent{});

Update(ADVANCED_TIME_TRANSITION_TIME);

expectAnimsToBeSet({
    sut_.animationClipsNames_.disarmed.movement.run.forward
});
```

The helper does not automatically assert velocity or rotation unless those behaviors are checked separately.

When timing or transition order matters, identify the exact point at which the animation expectation is evaluated.

## 7. Use the test body as the behavioral specification

Read the entire failing test before modifying production code.

Interpret every event, update, and expectation in sequence.

For each operation, determine:

- What behavior is expected before it?
- What changes because of the event?
- What should happen during the subsequent update?
- Which state should handle the next event?
- Which animations should be set afterward?
- What movement and rotation should be applied?
- Does the event interrupt, replace, or continue the existing behavior?

Consider this example:

```cpp
TEST_F(CharacterControllerTests, Mixed_ArmedChangeDuringDiveDoge)
{
    EXPECT_CALL(
        physicsApiMock_,
        GetRotation(rigidbodyid)
    ).WillRepeatedly(Return(Rotation().value_));

    EXPECT_CALL(
        physicsApiMock_,
        GetVelocity(rigidbodyid)
    ).WillRepeatedly(Return(vec3(0)));

    EXPECT_CALL(
        physicsApiMock_,
        SetVelocityRigidbody(
            rigidbodyid,
            vec3(0.0, 0.0, DEFAULT_RUN_SPEED)
        )
    ).Times(AtLeast(1));

    handleEvent(MoveForwardEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({
        sut_.animationClipsNames_.disarmed.movement.run.forward
    });

    handleEvent(DodgeDiveEvent{});
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({
        sut_.animationClipsNames_.disarmed.dodgeDive
    });

    handleEvent(WeaponStateEvent{});
    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_TRANSITION_TIME);

    expectAnimsToBeSet({
        sut_.animationClipsNames_.equip.front(),
        sut_.animationClipsNames_.armed.movement.run.forward
    });

    Update(ADVANCED_TIME_CLIP_TIME);
    Update(ADVANCED_TIME_CLIP_TIME);
}
```

The intended sequence is:

1. Start forward movement while disarmed.
2. Advance time and verify the forward-running animation.
3. Dispatch a dodge-dive event.
4. Advance time and verify the dodge-dive animation.
5. Dispatch a weapon-state event during the scenario.
6. Advance time through a clip interval and a transition interval.
7. Verify the equip animation and armed forward-running animation.
8. Continue updating to exercise subsequent time-dependent behavior.

Investigate whether the weapon-state event is correctly handled in the current state and whether the subsequent state, animation, and movement behavior matches the intended design.

The exact internal states and transition semantics must be verified in the implementation. Do not invent state names or assume an implementation detail based only on the test name.

## 8. Determine whether the problem is local or shared

Use the distribution of failures to guide your investigation.

### One test fails

Prioritize:

- The exact event sequence.
- The state active when each event is dispatched.
- State-specific event handling.
- Animation selection and timing.
- Movement and rotation behavior for that scenario.

However, do not assume that one failing test necessarily means the bug is local.

### Several similar tests fail

Investigate shared logic, including:

- A common state transition.
- A shared event handler.
- Common animation-selection logic.
- Movement or rotation calculations.
- Shared timing assumptions or test helpers.

### Many unrelated tests fail

Prioritize common infrastructure and shared components, such as:

- Base-state behavior.
- Event dispatch.
- Update timing.
- Animation management.
- Physics API interactions.
- Fixture initialization or cleanup.

These are investigation priorities, not guarantees. Verify each hypothesis against the failing tests and implementation.

Use related passing tests as comparisons to determine which part of the behavior differs.

## 9. Inspect the relevant implementation

After identifying where the scenario diverges, follow the relevant production code.

Investigate the following path as applicable:

1. Event dispatch.
2. Current-state event handling.
3. Transition request and transition execution.
4. Destination-state initialization or entry behavior.
5. Animation selection and transition logic.
6. Movement and rotation updates.
7. Animation completion or other time-dependent logic.
8. State exit and cleanup behavior.

For mixed tests, inspect every state involved in the event sequence and the code connecting them.

Search for related tests that exercise the same event in different states or similar transitions with different events.

Compare a failing scenario with a passing scenario and identify the smallest meaningful difference.

Prefer established project conventions and intended state-machine behavior. Avoid broad refactoring when a targeted correction is sufficient.

## 10. Form a root-cause hypothesis before editing

Before making changes, write down a short hypothesis based on the available evidence.

A useful hypothesis identifies:

- The event or update after which behavior diverges.
- The state or component responsible.
- The expected behavior.
- The actual behavior.
- The code path that plausibly explains the difference.

For example:

"The weapon-state event is dispatched while the dodge-dive behavior is active. The expected armed movement animation is not selected after the subsequent updates. I need to verify how the current state handles the weapon-state event and whether the transition or animation completion logic prevents the expected result."

This is an investigation hypothesis, not a confirmed root cause.

Inspect the implementation and test output to confirm or reject it before editing.

## 11. Implement the smallest correct fix

When the cause is confirmed:

1. Change only the relevant logic.
2. Preserve existing behavior for unrelated states.
3. Avoid weakening assertions or mock expectations.
4. Do not remove an event, update, or animation expectation merely to make the test pass.
5. Do not change the expected result unless the original test is demonstrably incorrect or the intended behavior has legitimately changed.
6. Consider whether the fix affects other states or transitions.

If the failure comes from a production bug, fix the production behavior rather than hiding the failure in the test.

If the test itself is incorrect, explain why before changing it.

## 12. Build and verify

After editing:

1. Rebuild using `ninja` in the configured build directory.
2. Rerun the exact failing test.
3. Inspect the output rather than relying on the process exit code alone.
4. Run relevant neighboring tests.
5. If shared behavior changed, run a broader test selection or the complete suite when practical.
6. Report any tests that could not be executed.

If the test still fails, return to the first meaningful failure and revise the hypothesis.

Do not repeatedly make speculative changes without learning from the previous run.

A successful targeted test does not prove that shared state behavior is regression-free.

## 13. Debugging constraints

Always follow these rules:

- Do not modify unrelated code.
- Do not delete assertions or weaken mock expectations just to make a test pass.
- Do not assume an event guarantees an immediate transition.
- Do not ignore the sequence or duration of `Update` calls.
- Do not assume `Times(AtLeast(1))` guarantees sustained movement.
- Do not assume the state named in the test is the only state involved.
- Do not treat every downstream assertion failure as an independent problem.
- Do not assume the first plausible explanation is the root cause.
- Do not claim runtime behavior that is not supported by logs, assertions, or implementation.
- Do not claim that a fix has been verified unless the build and test were actually run.
- If evidence is insufficient, state what remains unknown and identify the smallest additional investigation needed.

## 14. Required final report

After investigating the failing test, provide a concise report using the following structure.

### Failure
- Test name and source file.
- First meaningful failure.
- Expected versus actual behavior.

### Scenario timeline
- Important events and updates in order.
- Expected state and animation after each relevant step.
- Observed transitions and behavior, based on available evidence.

### Root cause
- The specific state, event handler, transition, timing issue, animation selection, movement calculation, rotation calculation, or mock expectation responsible.
- Why the behavior differs from the intended result.
- Evidence supporting the diagnosis.

### Fix
- Files and logic changed.
- Why the change is correct.
- Potential side effects or remaining uncertainties.

### Verification
- Build result.
- Targeted test result.
- Relevant regression-test results.
- Tests that could not be run.

If the root cause is not confirmed, explicitly label it as a hypothesis and describe the next investigation step. Do not present an unverified explanation as a confirmed diagnosis.

## 15. Core debugging principle

**Follow the execution timeline, not just the failed assertion.**

Locate the test, understand its event sequence, trace the actual state transitions, and compare the expected animations, movement, rotation, and timing with the observed behavior.

For mixed-state tests, follow the complete interaction between states rather than investigating only one state in isolation.

Identify the underlying cause, make the smallest correct fix, and verify it with targeted and regression tests.
