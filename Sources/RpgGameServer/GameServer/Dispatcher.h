#pragma once
#include "Handlers/IHandler.h"
#include <vector>

namespace GameServer
{
	typedef std::vector<Handler::IHandlerPtr> Handlers;

	class Dispacher
	{
	public:
		void SetHandlers(Handlers handlers);
		void AddHandlers(Handlers handlers);
		void AddHandler(Handler::IHandlerPtr handlerPtr);
		void Dispatch(const Network::BoxMessage& message);

	private:
		Handlers handlers_;
	};
} // GameServer