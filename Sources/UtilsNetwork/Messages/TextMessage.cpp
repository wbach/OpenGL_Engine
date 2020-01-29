#include "TextMessage.h"

namespace Network
{
TextMessage::TextMessage()
    : IMessage(MessageTypes::Text, MessageTarget::Dedicated)
{
    memset(text_, 0, TEXT_MSG_ARRAY_SIZE);
}

TextMessage::TextMessage(const std::string &text)
    : IMessage(MessageTypes::Text, MessageTarget::Dedicated)
{
    memset(text_, 0, TEXT_MSG_ARRAY_SIZE);
    CopyToArray(text_, text, TEXT_MSG_ARRAY_SIZE);
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
