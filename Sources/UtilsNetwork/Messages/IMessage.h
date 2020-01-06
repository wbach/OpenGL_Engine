#pragma once
#include <Logger/Log.h>

#include <cstring>
#include <memory>

#include "MessageTypes.h"
#include "Types.h"

namespace Network
{
const uint16 arraySize = 256;
void CopyToArray(char* dest, const std::string& str);

struct MessageHeader
{
    uint8 msgType;
};

class IMessage
{
public:
    virtual ~IMessage() = default;
    virtual std::string ToString()
    {
        return std::string();
    }
    IMessage(uint8 type, uint8 target)
        : msgType(type)
        , target(target)
    {
    }
    uint8 GetType()
    {
        return msgType;
    }
    uint8 GetTarget()
    {
        return target;
    }

private:
    uint8 msgType;
    uint8 target;
};

typedef std::shared_ptr<IMessage> IMessagePtr;

template <class T>
static IMessagePtr CreateIMessagePtr(const T& msg)
{
    return std::make_shared<T>(msg);
}

template <class T>
std::shared_ptr<T> castMessageAs(IMessagePtr ptr)
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
}  // namespace Network
