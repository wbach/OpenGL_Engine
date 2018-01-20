#include "Dispatcher.h"

namespace GameServer
{
	void Dispacher::SetHandlers(Handlers handlers)
	{
		handlers_ = handlers;
	}
	void Dispacher::AddHandlers(Handlers handlers)
	{
		for(const auto& h : handlers)
			handlers_.push_back(h);
	}
	void Dispacher::AddHandler(Handler::IHandlerPtr handlerPtr)
	{
		handlers_.push_back(handlerPtr);
	}

	void Dispacher::Dispatch(const Network::BoxMessage& message)
	{
		for (auto& handler : handlers_)
			handler->Handle(message);
	}
} // GameServer