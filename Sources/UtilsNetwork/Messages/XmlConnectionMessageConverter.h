#pragma once
#include "UtilsNetwork/IMessageConverter.h"

namespace Network
{
class XmlConnectionMessageConverter : public IMessageConverter
{
public:
    XmlConnectionMessageConverter();
    virtual bool IsValid(IMessageFormat, IMessageType) const override;
    virtual std::unique_ptr<IMessage> Convert(IMessageType, const IMessageData&) override;
    virtual IMessageData Convert(const IMessage&) override;

private:
    IMessageData ConvertConnectionMessage(const IMessage&);
    IMessageData ConvertAuthenticationMessage(const IMessage&);
    IMessageData ConvertTextMessage(const IMessage&);
};
}  // namespace Network
