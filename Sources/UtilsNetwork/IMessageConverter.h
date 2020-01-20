#pragma once
#include <Types.h>
#include "IMessage.h"

namespace Network
{
class IMessageConverter
{
public:
    ~IMessageConverter() = default;
    virtual uint8 GetFormat() const = 0;
    virtual std::unique_ptr<IMessage> Convert(uint8 format, uint8 type, const std::vector<int8>& message) = 0;
    virtual std::vector<uint8> Convert(const IMessage& message) = 0;
};
}  // namespace Network
