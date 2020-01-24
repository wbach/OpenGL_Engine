#pragma once
#include <Types.h>
#include "IMessage.h"

namespace Network
{
typedef uint8 IMessageFormat;
typedef uint8 IMessageType;
typedef std::vector<int8> IMessageData;

class IMessageConverter
{
public:
    virtual ~IMessageConverter() = default;

    virtual bool IsValid(IMessageFormat, IMessageType) const                              = 0;
    virtual std::unique_ptr<IMessage> Convert(Network::IMessageType, const IMessageData&) = 0;
    virtual std::vector<int8> Convert(const IMessage&)                                    = 0;
};
}  // namespace Network
