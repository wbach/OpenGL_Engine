#pragma once
#include "Types.h"
#include "CharacterActions.h"
#include "../IController.h"
#include "../../Transform.h"
#include <list>

//#define GetTime() 
#define DurationToDoubleMs(x) std::chrono::duration<double, std::milli>(x).count()
#define DurationToFloatMs(x) std::chrono::duration<float, std::milli>(x).count()

namespace common
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

	template<class T, class D>
	struct SEventInfo
	{
		T startValue;
		T currentValue;
		D direction;
		float startTime;
		float endTime;
	};

	class CharacterController : public IController
	{
		typedef std::function<void (const vec3&)> OnUpdate;

	public:
		CharacterController(common::Transform& transform, float runSpeed, float turnSpeed, float jumpPower);
		virtual void Update(float dt) override;
		void AddState(CharacterActions::Type action);
		void RemoveState(CharacterActions::Type action);
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

		template<class T, class D>
		T CalculateNewValueInTimeInterval(SEventInfo<T, D>& t, float time)
		{
			float totalMoveTime = t.endTime - t.startTime;

			if (fabs(totalMoveTime) < FLT_EPSILON)
			{
				return t.startValue;
			}

			return t.startValue + t.currentValue * (time - t.startTime) / totalMoveTime;
		}

	private:
		Timepoint referenceTime;
		CharacterActions::Type action = CharacterActions::IDLE;
		common::Transform& transform_;

		float moveTime_;
		float runSpeed_;
		float turnSpeed_;
		float jumpPower_;
		bool isGrounded;
		float upwardsSpeed = 0.f;

		std::list<CharacterActions::Type> states;

		SEventInfo<vec2, Direction>  moveStateInfo;
		SEventInfo<float, RotationDirection> rotateStateInfo;
	};
	} // Controllers
} // common
