#pragma once
#include "UtilsNetwork/IMessage.h"
#include "UtilsNetwork/MessageTarget.h"
#include "UtilsNetwork/Messages/MessageTypes.h"

namespace Network
{
class TextMessage : public IMessage
{
public:
    TextMessage();
    TextMessage(const std::string& text);

    std::string GetText() const;
    virtual std::string ToString() const override;

private:
    char text_[arraySize];
};
}  // namespace Network
