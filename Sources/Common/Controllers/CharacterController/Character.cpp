#include "Character.h"
#include <algorithm>
#include "../../Transform.h"

namespace common
{
namespace Controllers
{
CharacterController::CharacterController(common::Transform& transform, float runSpeed, float turnSpeed, float jumpPower)
    : IController(CharacterControllerType)
    , transform_(transform)
    , moveTime_(1000.0f)
    , runSpeed_(runSpeed)
    , turnSpeed_(turnSpeed)
    , jumpPower_(jumpPower)
    , isGrounded(false)
    , upwardsSpeed(0.f)
{
    referenceTime = std::chrono::high_resolution_clock::now();
}

void CharacterController::SetPosition(const glm::vec3& p)
{
    transform_.SetPosition(p);
}

void CharacterController::SetJumpPower(float v)
{
    jumpPower_ = v;
}

void CharacterController::SetTurnSpeed(float v)
{
    turnSpeed_ = v;
}

void CharacterController::SetRunSpeed(float v)
{
    runSpeed_ = v;
}

void CharacterController::Update(float)
{
    float time = GetTime();

    for (auto state = states.begin(); state != states.end(); ++state)
        ProcessState(state, time);
}

void CharacterController::ProcessState(std::list<CharacterActions::Type>::iterator& state, float time)
{
    switch (*state)
    {
        case CharacterActions::MOVE_FORWARD:
            MoveState(state, time);
            break;
        case CharacterActions::MOVE_BACKWARD:
            MoveState(state, time);
            break;

        case CharacterActions::ROTATE_LEFT:
            RotateState(state, time);
            break;
        case CharacterActions::ROTATE_RIGHT:
            RotateState(state, time);
            break;
        default:
            break;
    }
}

void CharacterController::Jump()
{
    if (!isGrounded)
        return;

    upwardsSpeed = jumpPower_;
    isGrounded   = false;
}

void CharacterController::AddState(CharacterActions::Type action)
{
    if (action == CharacterActions::MOVE_FORWARD)
        MoveForward();

    if (action == CharacterActions::MOVE_BACKWARD)
        MoveBackward();

    if (action == CharacterActions::ROTATE_LEFT)
        RotateLeft();

    if (action == CharacterActions::ROTATE_RIGHT)
        RotateRight();
}

void CharacterController::RemoveState(CharacterActions::Type action)
{
    states.remove(action);
}

common::Transform& CharacterController::GetTransform()
{
    return transform_;
}

float CharacterController::GetTime() const
{
    auto currnet = std::chrono::high_resolution_clock::now() - referenceTime;
    return DurationToFloatMs(currnet) / 1000.0f;
}

void CharacterController::SetRotateStateInfo(RotationDirection dir)
{
    rotateStateInfo.startValue   = transform_.GetRotation().GetEulerDegrees()->y;
    rotateStateInfo.currentValue = turnSpeed_ * moveTime_ * (dir == RotationDirection::RIGHT ? -1.f : 1.f);
    rotateStateInfo.startTime    = GetTime();
    rotateStateInfo.direction    = dir;
    rotateStateInfo.endTime      = rotateStateInfo.startTime + moveTime_;
}

void CharacterController::RotateLeft()
{
    if (FindState(CharacterActions::ROTATE_LEFT))
        return;

    states.remove(CharacterActions::ROTATE_RIGHT);
    states.push_front(CharacterActions::ROTATE_LEFT);
    SetRotateStateInfo(RotationDirection::LEFT);
}

void CharacterController::RotateRight()
{
    if (FindState(CharacterActions::ROTATE_RIGHT))
        return;

    states.remove(CharacterActions::ROTATE_LEFT);
    states.push_front(CharacterActions::ROTATE_RIGHT);
    SetRotateStateInfo(RotationDirection::RIGHT);
}

void CharacterController::RotateState(std::list<CharacterActions::Type>::iterator& state, float time)
{
    float rotateValue = CalculateNewValueInTimeInterval<float, RotationDirection>(rotateStateInfo, time);

    LockRotate(rotateValue);

    transform_.SetRotate(common::Axis::Y, rotateValue);
    RemoveStateIfTimeElapsed(state, time, rotateStateInfo.endTime);
}

void CharacterController::RemoveStateIfTimeElapsed(std::list<CharacterActions::Type>::iterator& state, float time,
                                                   float endTime)
{
    if (time > endTime)
        state = states.erase(state);
}

void CharacterController::LockRotate(float& rotate)
{
    if (rotate < 0.f)
        rotate += 360.0f;

    if (rotate > 360.0f)
        rotate -= 360.0f;
}

void CharacterController::MoveForward()
{
    if (FindState(CharacterActions::MOVE_FORWARD))
        return;

    states.remove(CharacterActions::MOVE_BACKWARD);
    states.push_front(CharacterActions::MOVE_FORWARD);

    moveStateInfo.startValue   = transform_.GetPositionXZ();
    moveStateInfo.currentValue = CalculateMoveVector(Direction::FORWARD) * moveTime_;
    moveStateInfo.direction    = Direction::FORWARD;
    moveStateInfo.startTime    = GetTime();
    moveStateInfo.endTime      = moveStateInfo.startTime + moveTime_;
}

void CharacterController::MoveBackward()
{
    if (FindState(CharacterActions::MOVE_BACKWARD))
        return;

    states.remove(CharacterActions::MOVE_FORWARD);
    states.push_front(CharacterActions::MOVE_BACKWARD);

    moveStateInfo.startValue   = transform_.GetPositionXZ();
    moveStateInfo.currentValue = CalculateMoveVector(Direction::BACKWARD) * moveTime_;
    moveStateInfo.direction    = Direction::BACKWARD;
    moveStateInfo.startTime    = GetTime();
    moveStateInfo.endTime      = moveStateInfo.startTime + moveTime_;
}

void CharacterController::MoveState(std::list<CharacterActions::Type>::iterator& state, float time)
{
    vec2 posValue = CalculateNewValueInTimeInterval<vec2, Direction>(moveStateInfo, time);

    transform_.SetPositionXZ(posValue);
    RemoveStateIfTimeElapsed(state, time, moveStateInfo.endTime);

    float dt                   = (moveStateInfo.endTime - time) / moveTime_;
    moveStateInfo.startValue   = transform_.GetPositionXZ();
    moveStateInfo.currentValue = CalculateMoveVector(moveStateInfo.direction) * moveTime_;
    moveStateInfo.currentValue *= dt;
    moveStateInfo.startTime = time;
}

vec2 CharacterController::CalculateMoveVector(Direction direction)
{
    float rad = transform_.GetRotation().GetEulerRadians()->y;
    auto v    = vec2(runSpeed_ * (direction == Direction::BACKWARD ? -1.f : 1.f));
    v.x *= sinf(rad);
    v.y *= cosf(rad);

    return v;
}

bool CharacterController::FindState(CharacterActions::Type state)
{
    return std::find(states.begin(), states.end(), state) != states.end();
}
}  // namespace Controllers
}  // namespace common
