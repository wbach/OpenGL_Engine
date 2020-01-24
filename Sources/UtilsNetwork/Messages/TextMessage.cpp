#include "TextMessage.h"

namespace Network
{
TextMessage::TextMessage()
    : IMessage(MessageTypes::Text, MessageTarget::Dedicated)
{
    memset(text_, 0, arraySize);
}

TextMessage::TextMessage(const std::string &text)
    : IMessage(MessageTypes::Text, MessageTarget::Dedicated)
{
    memset(text_, 0, arraySize);
    CopyToArray(text_, text);
}

std::string TextMessage::GetText() const
{
    return text_;
}

std::string TextMessage::ToString() const
{
    return text_;
}

}  // namespace Network
