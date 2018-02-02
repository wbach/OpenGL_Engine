#pragma once
#include  "ControllersTypes.h"
#include "../Hero/Stats.h"
#include <memory>

namespace common
{
	class Transform;

	namespace Controllers
	{
		class IController;
		std::shared_ptr<IController> CreateCharacterController(Transform& attachedTranform, const Hero::CommonStats& stats);
	} // Controllers
} // common