#pragma once
#include "Message.h"
#include "NetworkTypes.h"

namespace GameServer
{
	namespace Controllers
	{
		class IController
		{
		public:
			virtual ~IController() {}
			virtual void Update(float dt) = 0;
			virtual void Handle(const Network::BoxMessage& mesage) = 0;

		protected:
			bool CheckType(const Network::BoxMessage& mesage)
			{
				return mesage.second->GetType() == type_;
			}

		private:
			Network::MessageTypes type_;
		};

		typedef std::shared_ptr<IController> IControllerPtr;
	} // Controllers
} // GameServer