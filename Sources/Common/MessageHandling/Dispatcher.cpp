#include "Dispatcher.h"


namespace common
{
	void Dispacher::SetHandlers(Handlers handlers)
	{
		handlers_ = handlers;
	}
	void Dispacher::AddHandlers(Handlers handlers)
	{
		for (const auto& h : handlers)
			handlers_[h.first] = h.second;
	}
	void Dispacher::AddHandler(const std::string& label, AbstractHandler* handlerPtr)
	{
		handlers_[label] = std::shared_ptr<AbstractHandler>(handlerPtr);
	}
	void Dispacher::Dispatch(const Network::BoxMessage& message)
	{
		bool result = false;
		for (auto& handler : handlers_)
		{
			if (handler.second->Handle(message))
				result = true;
		}

		if (!result)
			DEBUG_LOG("Dispacher::Dispatch message missed.");
	}
	void Dispacher::RemoveHandler(const std::string& label)
	{
		if (handlers_.count(label) == 0)
			return;

		handlers_.erase(label);
	}
	void Dispacher::Clear()
	{
		handlers_.clear();
	}

} // GameServer