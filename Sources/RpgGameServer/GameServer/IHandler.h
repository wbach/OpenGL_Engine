#pragma once
#include "User.h"
#include "Message.h"
#include "Gateway.h"
#include "User.h"

namespace GameServer
{
	class IHandler
	{
	public:
		IHandler(Network::MessageTypes type)
			: type_(type)
		{}
		virtual ~IHandler(){}
		virtual void Handle(User& destUser, const Network::BoxMessage& mesage) = 0;

		bool CheckType(const Network::BoxMessage& mesage)
		{
			return mesage.second->GetType() == type_;
		}

	private:
		Network::MessageTypes type_;
	};
} // GameServer