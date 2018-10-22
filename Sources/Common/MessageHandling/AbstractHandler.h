#pragma once
#include "Logger/Log.h"
#include "UtilsNetwork/NetworkTypes.h"

namespace common
{
typedef std::vector<Network::MessageTypes> CompatibilityTypes;
class AbstractHandler
{
public:
    AbstractHandler()
    {
    }
    AbstractHandler(Network::MessageTypes type)
        : types_({type})
    {
    }
    AbstractHandler(CompatibilityTypes types)
        : types_(types)
    {
    }

    virtual ~AbstractHandler()
    {
    }

    bool Handle(const Network::BoxMessage& message)
    {
        if (!CheckType(message))
            return false;

        ProcessMessage(message);

        Log(Network::to_string(message.second->GetType()));

        return true;
    }

    template <class T>
    T* CastToMsgType(Network::IMessagePtr message)
    {
        auto msg = dynamic_cast<T*>(message.get());

        if (msg == nullptr)
        {
            Log("[Error] CastToMsgType: cannot cast " + std::to_string(message->GetType()) + ".");
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

typedef std::shared_ptr<AbstractHandler> AbstractHandlerPtr;
}  // GameServer
