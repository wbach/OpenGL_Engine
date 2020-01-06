#pragma once
#include "../IMessage.h"
#include "../MessageTarget.h"

namespace Network
{
class OtherMsg : public IMessage
{
public:
    OtherMsg()
        : IMessage(MessageTypes::Other, MessageTarget::Dedicated)
    {
        memset(data_, 0, arraySize);
    }

    OtherMsg(const std::string& data)
        : IMessage(MessageTypes::Other, MessageTarget::Dedicated)
    {
        memset(data_, 0, arraySize);
        CopyToArray(data_, data);
    }

    std::string GetData()
    {
        return data_;
    }

    virtual std::string ToString() override
    {
        return "OtherMsg";
    }

private:
    char data_[arraySize];
};
}  // namespace Network