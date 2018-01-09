#pragma once
#include "../GameEngine/Input/InputManager.h"
#include "../GameEngine/Objects/RenderAble/Entity/Entity.h"
#include "CharacterStats.h"
#include <list>

//#define GetTime() 
#define DurationToDoubleMs(x) std::chrono::duration<double, std::milli>(x).count()
#define DurationToFloatMs(x) std::chrono::duration<float, std::milli>(x).count()

enum class Direction
{
	FORWARD,
	BACKWARD
};

enum class RotationDirection
{
	LEFT,
	RIGHT
};

namespace CharacterActions
{
	enum Type
	{
		IDLE = 0,
		MOVE_FORWARD,
		MOVE_BACKWARD,
		ROTATE_LEFT,
		ROTATE_RIGHT,
		RUN,
		WALK,
		ATTACK_1,
		ATTACK_2,
		ATTACK_3,
		SPELL_1,
		SPELL_2,
		SPELL_3,
		SPELL_4,
		COUNT
	};
}

template<class T>
struct SEventInfo
{
	T startValue;
	T currentValue;
	float startTime;
	float endTime;
};

class CPlayer : public CEntity
{
public:
    CPlayer(CInputManager* input_manager, CResourceManager& manager, const glm::vec3& normalized_scale, const std::string& filename);
	
    CharacterActions::Type GetAction() { return action; }
	void SetAction(CharacterActions::Type a);
	void SetPosition(const glm::vec3& p);

	void Update(float deltaTime);
	void ProcessState(std::list<CharacterActions::Type>::iterator& state);
	bool FindState(CharacterActions::Type state);

	void Jump();
	void CheckInputs();

private:
	void MoveInputs();
	float GetTime() const;
	void SetRotateStateInfo(RotationDirection direction);
	void RotateLeft();
	void RotateRight();
	void RotateState(std::list<CharacterActions::Type>::iterator& state, float time);
	void RemoveStateIfTimeElapsed(std::list<CharacterActions::Type>::iterator& state, float time, float endTime);
	void LockRotate(float& rotate);
	void MoveForward();
	void MoveBackward();
	void MoveState(std::list<CharacterActions::Type>::iterator& state, float time);
	vec2 CalculateMoveVector(Direction direction);


	template<class T>
	T CalculateNewValueInTimeInterval(SEventInfo<T>& t, float time)
	{
		return t.startValue + t.currentValue * (time - t.startTime) / (t.endTime - t.startTime);
	}

private:
	Timepoint referenceTime;
    CInputManager* inputManager;
    CharacterActions::Type action = CharacterActions::IDLE;
    SCharacterStats characterStats;

    bool isGrounded = false;
	float upwardsSpeed = 0.f;

	std::list<CharacterActions::Type> states;

	SEventInfo<vec2>  moveStateInfo;
	SEventInfo<float> rotateStateInfo;
};
