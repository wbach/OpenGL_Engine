#include "Character.h"
#include "Logger/Log.h"
#include "math.hpp"

namespace GameServer
{
	namespace Controllers
	{
		CharacterController::CharacterController(common::Transform& transform, float runSpeed, float turnSpeed, float jumpPower)
			: IController(CharacterControllerType)
			, transform_(transform)
			, turnSpeed_(turnSpeed)
			, jumpPower_(jumpPower)
			, runSpeed_(runSpeed)
		{
			referenceTime = std::chrono::high_resolution_clock::now();
		}

		void CharacterController::SetPosition(const glm::vec3 & p)
		{
		    transform_.SetPosition(p);
		}

		void CharacterController::Update(float deltaTime)
		{
			float time = GetTime();

			for (auto state = states.begin(); state != states.end(); ++state)
				ProcessState(state, time);
		}

		void CharacterController::ProcessState(std::list<CharacterActions::Type>::iterator& state, float time)
		{
			switch (*state)
			{
			case CharacterActions::MOVE_FORWARD:  MoveState(state, time); break;
			case CharacterActions::MOVE_BACKWARD: MoveState(state, time); break;

			case CharacterActions::ROTATE_LEFT:	 RotateState(state, time); break;
			case CharacterActions::ROTATE_RIGHT: RotateState(state, time); break;
			}
		}

		void CharacterController::Jump()
		{
			if (!isGrounded)
				return;

			upwardsSpeed = jumpPower_;
			isGrounded = false;
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

		common::Transform & CharacterController::GetTransform()
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
			float timeInterval = 0.05f;
			rotateStateInfo.startValue		= transform_.GetRotation().y;
			rotateStateInfo.currentValue	= turnSpeed_ * timeInterval * (dir == RotationDirection::RIGHT ? -1.f : 1.f);
			rotateStateInfo.startTime		= GetTime();
			rotateStateInfo.endTime			= rotateStateInfo.startTime + timeInterval;
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
			float rotateValue = CalculateNewValueInTimeInterval<float>(rotateStateInfo, time);

			LockRotate(rotateValue);

			transform_.SetRotate(common::Axis::Y, rotateValue);
			RemoveStateIfTimeElapsed(state, time, rotateStateInfo.endTime);
		}

		void CharacterController::RemoveStateIfTimeElapsed(std::list<CharacterActions::Type>::iterator& state, float time, float endTime)
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

			float timeInterval = 0.05f;
			moveStateInfo.startValue	= transform_.GetPositionXZ();
			moveStateInfo.currentValue	= CalculateMoveVector(Direction::FORWARD) * timeInterval;
			moveStateInfo.startTime		= GetTime();
			moveStateInfo.endTime		= moveStateInfo.startTime + timeInterval;			
		}

		void CharacterController::MoveBackward()
		{
			if (FindState(CharacterActions::MOVE_BACKWARD))
				return;

			states.remove(CharacterActions::MOVE_FORWARD);
			states.push_front(CharacterActions::MOVE_BACKWARD);

			float timeInterval = 0.05f;
			moveStateInfo.startValue	= transform_.GetPositionXZ();
			moveStateInfo.currentValue	= CalculateMoveVector(Direction::BACKWARD) * timeInterval;
			moveStateInfo.startTime		= GetTime();
			moveStateInfo.endTime		= moveStateInfo.startTime + timeInterval;
		}

		void CharacterController::MoveState(std::list<CharacterActions::Type>::iterator & state, float time)
		{
			vec2 rotateValue = CalculateNewValueInTimeInterval<vec2>(moveStateInfo, time);
			transform_.SetPosition(vec3(rotateValue.x, 0, rotateValue.y));
			RemoveStateIfTimeElapsed(state, time, moveStateInfo.endTime);
		}

		vec2 CharacterController::CalculateMoveVector(Direction direction)
		{
			float rad = Utils::ToRadians(transform_.GetRotation().y);
			auto v = vec2(runSpeed_ * (direction == Direction::BACKWARD ? -1.f : 1.f) );
			v.x *= sin(rad);
			v.y *= cos(rad);

			return v;
		}

		bool CharacterController::FindState(CharacterActions::Type state)
		{
			for (auto state_it = states.begin(); state_it != states.end(); ++state_it)
			{
				if (*state_it == state)
					return true;
			}
			return false;
		}
	} // Controllers
} // GameServer