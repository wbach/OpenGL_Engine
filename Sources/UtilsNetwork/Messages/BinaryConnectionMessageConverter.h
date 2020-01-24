#pragma once
#include "IMessageConverter.h"

namespace Network
{
class BinaryConnectionMessageConverter : public IMessageConverter
{
public:
    BinaryConnectionMessageConverter();
    virtual bool IsValid(IMessageFormat, IMessageType) const override;
    virtual std::unique_ptr<IMessage> Convert(IMessageType, const IMessageData&) override;
    virtual IMessageData Convert(const IMessage&) override;

private:
    template<class T>
    std::vector<int8> ConvertMessage(const IMessage&);
    template<class T>
    std::unique_ptr<IMessage> ConvertMessage(const IMessageData&);
};
}  // namespace Network
