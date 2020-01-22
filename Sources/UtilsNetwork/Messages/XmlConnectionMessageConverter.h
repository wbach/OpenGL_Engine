#pragma once
#include "IMessageConverter.h"

namespace Network
{
class XmlConnectionMessageConverter : public IMessageConverter
{
public:
    XmlConnectionMessageConverter();
    virtual uint8 GetFormat() const override;
    virtual std::unique_ptr<IMessage> Convert(uint8 type, const std::vector<int8>& message) override;
    virtual std::vector<int8> Convert(const IMessage& message) override;

private:
    std::unique_ptr<IMessage> ConvertXml(uint8 type, const std::vector<int8>& message);

private:
    std::vector<int8> ConvertConnectionMessage(const IMessage& message);
    std::vector<int8> ConvertAuthenticationMessage(const IMessage& message);
};
}  // namespace Network
