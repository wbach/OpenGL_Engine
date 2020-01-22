#pragma once
#include "IMessageConverter.h"

namespace Network
{
class BinaryConnectionMessageConverter : public IMessageConverter
{
public:
    BinaryConnectionMessageConverter();
    virtual uint8 GetFormat() const override;
    virtual std::unique_ptr<IMessage> Convert(uint8 type, const std::vector<int8>& message) override;
    virtual std::vector<int8> Convert(const IMessage& message) override;

private:
    std::vector<int8> ConvertConnectionMessage(const IMessage& message);
    std::vector<int8> ConvertAuthenticationMessage(const IMessage& message);
    std::vector<int8> ConvertTextMessage(const IMessage& message);
};
}  // namespace Network
