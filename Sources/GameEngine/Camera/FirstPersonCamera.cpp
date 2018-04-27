#include "FirstPersonCamera.h"
#include "../Input/InputManager.h"
#include "../Display/DisplayManager.hpp"
#include "math.hpp"

static vec3 zero(0);

const float defaultCamSpeed = Utils::KmToMs<float>(1);
const float defaultCamRotationSpeed = 0.2f;

namespace GameEngine
{
	FirstPersonCamera::FirstPersonCamera(GameEngine::InputManager* input_manager, GameEngine::CDisplayManager *display_manager)
		: FirstPersonCamera(input_manager, display_manager, defaultCamRotationSpeed, defaultCamSpeed, zero, zero, true)
	{
	}

	FirstPersonCamera::FirstPersonCamera(GameEngine::InputManager* input_manager, GameEngine::CDisplayManager * display_manager, float mouse_velocity, float move_velocity)
		: FirstPersonCamera(input_manager, display_manager, mouse_velocity, move_velocity, zero, zero, false)
	{
	}

	FirstPersonCamera::FirstPersonCamera(GameEngine::InputManager* input_manager, GameEngine::CDisplayManager *display_manager, vec3& position_entity, vec3& rotation_entity)
		: FirstPersonCamera(input_manager, display_manager, defaultCamRotationSpeed, defaultCamSpeed, position_entity, rotation_entity, false)
	{
	}

	FirstPersonCamera::FirstPersonCamera(GameEngine::InputManager* input_manager, GameEngine::CDisplayManager * display_manager, float mouse_velocity, float move_velocity, vec3 & position_entity, vec3 & rotation_entity, bool freeCamera)
		: BaseCamera(9.f, 100.f)
		, inputManager(input_manager)
		, displayManager(display_manager)
		, lookPosition(position_entity)
		, lookRotation(rotation_entity)
		, isFreeCamera(freeCamera)
		, mousevel(mouse_velocity)
		, movevel(move_velocity)
	{
	}

	void FirstPersonCamera::LockCamera()
	{
		LockPitch();
		LockYaw();
	}

	void FirstPersonCamera::LockPitch()
	{
		float p = pitch.load();

		if (p > 90.f)
			pitch.store(90.f);
		if (p < -90.f)
			pitch.store(-90.f);
	}

	void FirstPersonCamera::LockYaw()
	{
		float y = yaw.load();

		if (y < 0.f)
			yaw.store(y + 360.f);
		if (y > 360.f)
			yaw.store(y - 360.f);
	}

	void FirstPersonCamera::Move()
	{
		ApllyMove();
		CalculateMoveVelocity();
		CheckAndProccesDirections();
	}

	vec2 FirstPersonCamera::CalcualteMouseMove()
	{
		vec2 d_move = inputManager->CalcualteMouseMove() * mousevel;
		return d_move;
	}

	void FirstPersonCamera::ApllyMove()
	{
		if (!inputManager->GetMouseKey(KeyCodes::LMOUSE))
			return;

		vec2 dmove = CalcualteMouseMove();
		yaw.store(yaw.load() - dmove.x);// *deltaTime;
		pitch.store(pitch.load() - dmove.y);// *deltaTime;
		LockCamera();
	}

	void FirstPersonCamera::CalculateMoveVelocity()
	{
		currentMoveVelocity = movevel;// *static_cast<float>(displayManager->GetDeltaTime());
	}

	void FirstPersonCamera::CheckAndProccesDirections()
	{
		CheckAndProccesUpDirection();
		CheckAndProccesDownDirection();

		CheckAndProccesLeftDirection();
		CheckAndProccesRightDirection();
	}

	bool FirstPersonCamera::CheckAndProccesUpDirection()
	{
		if (!inputManager->GetKey(KeyCodes::UARROW))
			return false;

		if (pitch.load() != 90.f && pitch.load() != -90.f)
			MoveCamera(currentMoveVelocity, 0.f);

		MoveCameraUp(currentMoveVelocity, 0.f);
		return true;
	}

	bool FirstPersonCamera::CheckAndProccesDownDirection()
	{
		if (!inputManager->GetKey(KeyCodes::DARROW))
			return false;

		if (pitch.load() != 90.f && pitch.load() != -90.f)
			MoveCamera(currentMoveVelocity, 180.f);

		MoveCameraUp(currentMoveVelocity, 180.f);
		return true;
	}

	bool FirstPersonCamera::CheckAndProccesLeftDirection()
	{
		if (!inputManager->GetKey(KeyCodes::LARROW))
			return false;

		MoveCamera(-currentMoveVelocity, 90.f);
		return true;
	}

	bool FirstPersonCamera::CheckAndProccesRightDirection()
	{
		if (!inputManager->GetKey(KeyCodes::RARROW))
			return false;

		MoveCamera(-currentMoveVelocity, 270.f);
		return true;
	}

	void FirstPersonCamera::MoveCamera(float dist, float dir)
	{
		float rad = (yaw.load() + dir)*static_cast<float>(M_PI) / 180.f;
		position.x -= sin(-rad)*dist;
		position.z -= cos(-rad)*dist;
	}

	void FirstPersonCamera::MoveCameraUp(float dist, float dir)
	{
		float rad = (pitch.load() + dir)*static_cast<float>(M_PI) / 180.f;
		position.y += sin(-rad)*dist;
	}
} // GameEngine
