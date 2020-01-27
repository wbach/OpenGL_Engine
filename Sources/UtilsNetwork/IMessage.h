#pragma once
#include <Logger/Log.h>

#include <cstring>
#include <memory>

#include "Messages/MessageTypes.h"
#include "Types.h"

namespace Network
{
typedef uint8 IMessageFormat;
typedef uint8 IMessageType;
typedef uint8 IMessageTarget;
typedef std::vector<int8> IMessageData;

const uint16 arraySize = 256;
void CopyToArray(char* dest, const std::string& str, int size = arraySize - 1);

struct MessageHeader
{
    IMessageType msgType;
};

class IMessage
{
public:
    virtual ~IMessage() = default;
    virtual std::string ToString() const
    {
        return "---";
    }
    IMessage(IMessageType type, IMessageTarget target)
        : msgType(type)
        , target(target)
    {
    }
    IMessageType GetType() const
    {
        return msgType;
    }
    IMessageTarget GetTarget() const
    {
        return target;
    }

private:
    IMessageType msgType;
    IMessageTarget target;
};

template <class T>
std::unique_ptr<T> castMessageAs(std::unique_ptr<IMessage> ptr)
{
    auto msg = std::static_pointer_cast<T>(ptr);
    if (msg == nullptr)
    {
        ERROR_LOG("Cant cast type : " + std::to_string(ptr->GetType()));
        return nullptr;
    }
    return msg;
}
template <class T>
T* castMessageAs(IMessage* ptr)
{
    auto msg = static_cast<T*>(ptr);
    if (msg == nullptr)
    {
        ERROR_LOG("Cant cast type : " + std::to_string(ptr->GetType()));
        return nullptr;
    }
    return msg;
}
template <class T>
const T* const castMessageAs(const IMessage& ptr)
{
    auto msg = &static_cast<const T&>(ptr);
    if (msg == nullptr)
    {
        ERROR_LOG("Cant cast type : " + std::to_string(ptr.GetType()));
        return nullptr;
    }
    return msg;
}
}  // namespace Network
