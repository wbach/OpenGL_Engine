#pragma once
#include <Logger/Log.h>
#include <UtilsNetwork/NetworkTypes.h>

#include "Common/Messages/MessageTypes.h"

namespace common
{
typedef std::vector<common::MessageTypes> CompatibilityTypes;

class AbstractHandler
{
public:
    AbstractHandler()
    {
    }
    AbstractHandler(common::MessageTypes type)
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

    bool Handle(uint32 userId, const Network::IMessage& message)
    {
        if (!CheckType(message))
            return false;

        ProcessMessage(userId, message);

        LOG_DEBUG << "Handle type: " << message.GetType();

        return true;
    }

    template <class T>
    T* CastToMsgType(Network::IMessage* message)
    {
        auto msg = dynamic_cast<T*>(message);

        if (msg == nullptr)
        {
            LOG_DEBUG << "[Error] CastToMsgType: cannot cast " << message->GetType() << ".";
            return nullptr;
        }

        return msg;
    }

protected:
    virtual void ProcessMessage(Network::UserId userId, const Network::IMessage& message) = 0;

    bool CheckType(const Network::IMessage& mesage)
    {
        for (auto type : types_)
        {
            if (type == common::MessageTypes::Any)
                return true;

            if (mesage.GetType() == type)
                return true;
        }

        return false;
    }

private:
    CompatibilityTypes types_;
};

typedef std::shared_ptr<AbstractHandler> AbstractHandlerPtr;
}  // namespace common
