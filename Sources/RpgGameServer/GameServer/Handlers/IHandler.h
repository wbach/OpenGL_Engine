#pragma once
#include "User.h"
#include "Gateway.h"

namespace GameServer
{
	namespace Handler
	{
		typedef std::vector<Network::MessageTypes> CompatibilityTypes;
		class IHandler
		{
		public:
			IHandler() 
			{}
			IHandler(Network::MessageTypes type)
				: types_({ type })
			{}
			IHandler(CompatibilityTypes types)
				: types_(types)
			{}

			virtual ~IHandler() {}

			void Handle(const Network::BoxMessage& message) 
			{
				if (!CheckType(message))
					return;

				ProcessMessage(message);
			};

			template <class T>
			T* CastToMsgType(Network::IMessagePtr message)
			{
				auto msg = dynamic_cast<T*>(message.get());

				if (msg == nullptr)
				{
					Log("[Error] CastToMsgType: cannot cast " + std::to_string(message->GetType()) +  ".");
					return nullptr;
				}

				return msg;
			}


		protected:
			virtual void ProcessMessage(const Network::BoxMessage& message) = 0;

			bool CheckType(const Network::BoxMessage& mesage)
			{
				for (auto type : types_)
				{
					if (type == Network::MessageTypes::Any)
						return true;

					if (mesage.second->GetType() == type)
						return true;
				}

				return false;
			}

		private:
			CompatibilityTypes types_;
		};

		typedef std::shared_ptr<IHandler> IHandlerPtr;
	} // Handler
} // GameServer