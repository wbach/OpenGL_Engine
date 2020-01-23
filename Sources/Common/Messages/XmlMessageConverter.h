#pragma once
#include <UtilsNetwork/IMessageConverter.h>

namespace common
{
class XmlMessageConverter : public Network::IMessageConverter
{
public:
    XmlMessageConverter();
    virtual bool IsValid(uint8, uint8) const override;
    virtual std::unique_ptr<IMessage> Convert(uint8 type, const std::vector<int8>& message) override;
    virtual std::vector<int8> Convert(const IMessage& message) override;

private:
    std::vector<int8> ConvertConnectionMessage(const IMessage& message);
    std::vector<int8> ConvertAuthenticationMessage(const IMessage& message);
    std::vector<int8> ConvertTextMessage(const IMessage& message);
};
}  // namespace common
