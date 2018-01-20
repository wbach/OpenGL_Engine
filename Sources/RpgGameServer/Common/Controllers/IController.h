#pragma once
#include "ControllersTypes.h"
#include <memory>

namespace GameServer
{
	namespace Controllers
	{
		class IController
		{
		public:
			virtual ~IController() {}
			virtual void Update(float dt) = 0;
			Controllers::Types GetType() { return type_; }

		private:
			Controllers::Types type_;
		};

		typedef std::shared_ptr<IController> IControllerPtr;
	} // Controllers
} // GameServer