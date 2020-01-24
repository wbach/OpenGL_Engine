#pragma once
#include <Logger/Log.h>

#include <cstring>
#include <memory>

#include "Messages/MessageTypes.h"
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
    virtual std::string ToString() const
    {
        return "---";
    }
    IMessage(uint8 type, uint8 target)
        : msgType(type)
        , target(target)
    {
    }
    uint8 GetType() const
    {
        return msgType;
    }
    uint8 GetTarget() const
    {
        return target;
    }

private:
    uint8 msgType;
    uint8 target;
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
