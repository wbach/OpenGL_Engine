#pragma once

namespace Network
{
	enum TransformMessageTypes
	{
		NONE,
		MOVE_FORWARD,
		MOVE_BACKWARD,
		ROTATE_RIGHT,
		ROTATE_LEFT,
		JUMP
	};
	enum TransformAction
	{
		PUSH,
		POP
	};
} // Network
