#include "Player.h"
#include "Logger/Log.h"

CPlayer::CPlayer(CInputManager *input_manager, CResourceManager &manager, const glm::vec3 &normalized_scale, const std::string &filename)
    : CEntity(&manager, normalized_scale)
    , inputManager(input_manager)
{
	AddModel(filename, GameEngine::LevelOfDetail::L1);
	referenceTime = std::chrono::high_resolution_clock::now();
}

void CPlayer::SetAction(CharacterActions::Type a)
{
    action = a;
}

void CPlayer::SetPosition(const glm::vec3 & p)
{
    worldTransform.SetPosition(p);
}

void CPlayer::Update(float deltaTime)
{
	CheckInputs();
	//float time = std::chrono::high_resolution_clock::now();

	float time = GetTime();

	for (auto state = states.begin(); state != states.end(); ++state)
		ProcessState(state, time);
}

void CPlayer::ProcessState(std::list<CharacterActions::Type>::iterator& state, float time)
{
	switch (*state)
	{
	case CharacterActions::MOVE_FORWARD:  MoveState(state, time); break;
	case CharacterActions::MOVE_BACKWARD: MoveState(state, time); break;

	case CharacterActions::ROTATE_LEFT:	 RotateState(state, time); break;
	case CharacterActions::ROTATE_RIGHT: RotateState(state, time); break;
	}
}

void CPlayer::Jump()
{
	if (!isGrounded)
		return;

	upwardsSpeed = characterStats.jumpPower;
	isGrounded = false;
}

void CPlayer::CheckInputs()
{
	if (inputManager == nullptr) return;

	MoveInputs();
}

void CPlayer::MoveInputs()
{
	if (inputManager->GetKey(GameActions::MOVE_FORWARD))	
		MoveForward();
	
	if (inputManager->GetKey(GameActions::MOVE_BACKWARD))
		MoveBackward();

	if (inputManager->GetKey(GameActions::TURN_LEFT))
		RotateLeft();

	if (inputManager->GetKey(GameActions::TURN_RIGHT))
		RotateRight();
}

float CPlayer::GetTime() const
{
	auto currnet = std::chrono::high_resolution_clock::now() - referenceTime;
	return DurationToFloatMs(currnet) / 1000.0f;
}

void CPlayer::SetRotateStateInfo(RotationDirection dir)
{
	float timeInterval = 0.05f;
	rotateStateInfo.startValue		= worldTransform.GetRotation().y;
	rotateStateInfo.currentValue	= characterStats.turnSpeed * timeInterval * (dir == RotationDirection::RIGHT ? -1.f : 1.f);
	rotateStateInfo.startTime		= GetTime();
	rotateStateInfo.endTime			= rotateStateInfo.startTime + timeInterval;
}

void CPlayer::RotateLeft()
{
	if (FindState(CharacterActions::ROTATE_LEFT))
		return;

	states.remove(CharacterActions::ROTATE_RIGHT);
	states.push_front(CharacterActions::ROTATE_LEFT);
	SetRotateStateInfo(RotationDirection::LEFT);
}

void CPlayer::RotateRight()
{
	if (FindState(CharacterActions::ROTATE_RIGHT))
		return;

	states.remove(CharacterActions::ROTATE_LEFT);
	states.push_front(CharacterActions::ROTATE_RIGHT);
	SetRotateStateInfo(RotationDirection::RIGHT);	
}

void CPlayer::RotateState(std::list<CharacterActions::Type>::iterator & state, float time)
{
	float rotateValue = CalculateNewValueInTimeInterval<float>(rotateStateInfo, time);

	LockRotate(rotateValue);

	worldTransform.SetRotate(common::Axis::Y, rotateValue);
	RemoveStateIfTimeElapsed(state, time, rotateStateInfo.endTime);
}

void CPlayer::RemoveStateIfTimeElapsed(std::list<CharacterActions::Type>::iterator & state, float time, float endTime)
{
	if (time > endTime)
		state = states.erase(state);
}

void CPlayer::LockRotate(float& rotate)
{
	if (rotate < 0.f)
		rotate += 360.0f;

	if (rotate > 360.0f)
		rotate -= 360.0f;
}

void CPlayer::MoveForward()
{
	if (FindState(CharacterActions::MOVE_FORWARD))
		return;
	
	states.remove(CharacterActions::MOVE_BACKWARD);
	states.push_front(CharacterActions::MOVE_FORWARD);

	float timeInterval = 0.05f;
	moveStateInfo.startValue	= worldTransform.GetPositionXZ();
	moveStateInfo.currentValue	= CalculateMoveVector(Direction::FORWARD) * timeInterval;
	moveStateInfo.startTime		= GetTime();
	moveStateInfo.endTime		= moveStateInfo.startTime + timeInterval;
	
}

void CPlayer::MoveBackward()
{
	if (FindState(CharacterActions::MOVE_BACKWARD))
		return;

	states.remove(CharacterActions::MOVE_FORWARD);
	states.push_front(CharacterActions::MOVE_BACKWARD);

	float timeInterval = 0.05f;
	moveStateInfo.startValue	= worldTransform.GetPositionXZ();
	moveStateInfo.currentValue	= CalculateMoveVector(Direction::BACKWARD) * timeInterval;
	moveStateInfo.startTime		= GetTime();
	moveStateInfo.endTime		= moveStateInfo.startTime + timeInterval;
}

void CPlayer::MoveState(std::list<CharacterActions::Type>::iterator & state, float time)
{
	vec2 rotateValue = CalculateNewValueInTimeInterval<vec2>(moveStateInfo, time);
	worldTransform.SetPosition(vec3(rotateValue.x, 0, rotateValue.y));
	RemoveStateIfTimeElapsed(state, time, moveStateInfo.endTime);
}

vec2 CPlayer::CalculateMoveVector(Direction direction)
{
	float rad = Utils::ToRadians(worldTransform.GetRotation().y);
	auto v = vec2(characterStats.runSpeed * (direction == Direction::BACKWARD ? -1.f : 1.f) );
	v.x *= sin(rad);
	v.y *= cos(rad);

	return v;
}

bool CPlayer::FindState(CharacterActions::Type state)
{
	for (auto state_it = states.begin(); state_it != states.end(); ++state_it)
	{
		if (*state_it == state)
			return true;
	}
	return false;
}
