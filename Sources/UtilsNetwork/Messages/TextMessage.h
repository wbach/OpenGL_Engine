#pragma once
#include "UtilsNetwork/IMessage.h"
#include "UtilsNetwork/MessageTarget.h"
#include "UtilsNetwork/Messages/MessageTypes.h"

namespace Network
{
static const uint32 TEXT_MSG_ARRAY_SIZE{512};

class TextMessage : public IMessage
{
public:
    TextMessage();
    TextMessage(const std::string& text);

    std::string GetText() const;
    virtual std::string ToString() const override;

private:
    char text_[TEXT_MSG_ARRAY_SIZE];
};
}  // namespace Network
