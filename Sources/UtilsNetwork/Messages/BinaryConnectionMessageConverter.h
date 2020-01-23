#pragma once
#include "IMessageConverter.h"

namespace Network
{
class BinaryConnectionMessageConverter : public IMessageConverter
{
public:
    BinaryConnectionMessageConverter();
    virtual bool IsValid(uint8 format, uint8 type) const override;
    virtual std::unique_ptr<IMessage> Convert(uint8 type, const std::vector<int8>& message) override;
    virtual std::vector<int8> Convert(const IMessage& message) override;

private:
    template<class T>
    std::vector<int8> ConvertMessage(const IMessage& message);
    template<class T>
    std::unique_ptr<IMessage> ConvertMessage(const std::vector<int8>& message);
};
}  // namespace Network
