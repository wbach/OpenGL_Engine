#pragma once
#include "IController.h"
#include "CharacterActions.h"
#include "../Transform.h"
#include "Types.h"
#include <list>

//#define GetTime() 
#define DurationToDoubleMs(x) std::chrono::duration<double, std::milli>(x).count()
#define DurationToFloatMs(x) std::chrono::duration<float, std::milli>(x).count()

namespace GameServer
{
	namespace Controllers
	{
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

	template<class T>
	struct SEventInfo
	{
		T startValue;
		T currentValue;
		float startTime;
		float endTime;
	};

	class CharacterController : public IController
	{
	public:
		CharacterController(common::Transform& transform, float runSpeed, float turnSpeed, float jumpPower);
		virtual void Update(float dt) override;
		void AddState(CharacterActions::Type action);
		common::Transform& GetTransform();

	private:
		CharacterActions::Type GetAction() { return action; }
		void SetPosition(const glm::vec3& p);

		bool FindState(CharacterActions::Type state);
		void ProcessState(std::list<CharacterActions::Type>::iterator& state, float time);
		void Jump();

	private:
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
		CharacterActions::Type action = CharacterActions::IDLE;

		common::Transform& transform_;

		float runSpeed_ = 0.0f;
		float turnSpeed_ = 0.0f;
		float jumpPower_ = 0.0f;
		bool isGrounded = false;
		float upwardsSpeed = 0.f;

		std::list<CharacterActions::Type> states;

		SEventInfo<vec2>  moveStateInfo;
		SEventInfo<float> rotateStateInfo;
	};
	} // Controllers
} // GameServer
