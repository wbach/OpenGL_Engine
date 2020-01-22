#pragma once
#include "UtilsNetwork/IMessage.h"
#include "UtilsNetwork/MessageTarget.h"
#include "UtilsNetwork/Messages/MessageTypes.h"

namespace Network
{
class TextMessage : public IMessage
{
   public:
    TextMessage(const std::string& text)
        : IMessage(MessageTypes::Text, MessageTarget::Dedicated)
    {
        memset(text_, 0, arraySize);
        CopyToArray(text_, text);
    }

    std::string GetText() const
    {
        return text_;
    }

    virtual std::string ToString() override
    {
        return text_;
    }

   private:
    char text_[arraySize];
};
}  // namespace Network
