#pragma once
#include <UtilsNetwork/IMessageConverter.h>

namespace GameEngine
{
namespace DebugNetworkInterface
{
class XmlMessageConverter : public Network::IMessageConverter
{
public:
    virtual bool IsValid(Network::IMessageFormat, Network::IMessageType) const override;
    virtual std::unique_ptr<Network::IMessage> Convert(Network::IMessageType, const Network::IMessageData&) override;
    virtual Network::IMessageData Convert(const Network::IMessage&) override;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
