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
    {
        if (handlers_.count(h.first) == 0)
        {
            handlers_.insert({h.first, h.second});
        }
        else
        {
            LOG_DEBUG << "Handler already exist.";
        }
    }
}
void Dispacher::AddHandler(const std::string& label, AbstractHandler* handlerPtr)
{
    handlers_[label] = std::shared_ptr<AbstractHandler>(handlerPtr);
}
void Dispacher::Dispatch(uint32 userId, const Network::IMessage& message)
{
    bool result = false;
    for (auto& handler : handlers_)
    {
        if (handler.second->Handle(userId, message))
            result = true;
    }

    if (not result)
    {
        LOG_DEBUG << "Handler not found. Dispacher::Dispatch message missed.";
    }
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

}  // namespace common
