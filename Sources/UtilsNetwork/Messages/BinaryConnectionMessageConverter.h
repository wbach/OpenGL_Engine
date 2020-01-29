#pragma once
#include "UtilsNetwork/IMessageConverter.h"

namespace Network
{
class BinaryConnectionMessageConverter : public IMessageConverter
{
public:
    BinaryConnectionMessageConverter();
    virtual bool IsValid(IMessageFormat, IMessageType) const override;
    virtual std::unique_ptr<IMessage> Convert(IMessageType, const IMessageData&) override;
    virtual IMessageData Convert(const IMessage&) override;
};
}  // namespace Network
