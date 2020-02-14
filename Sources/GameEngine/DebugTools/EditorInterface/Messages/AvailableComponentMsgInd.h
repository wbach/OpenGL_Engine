#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "GameEngine/DebugTools/EditorInterface/MessageTypes.h"

namespace GameEngine
{
struct AvailableComponentMsgInd : public Network::IMessage
{
public:
    AvailableComponentMsgInd()
        : AvailableComponentMsgInd("")
    {

    }
    AvailableComponentMsgInd(const std::string& name)
        : IMessage(static_cast<uint8>(MessageTypes::AvailableComponentMsgInd), Network::MessageTarget::All)
    {
        Network::CopyToArray(name_, name);
    }

    std::string GetName() const
    {
        return name_;
    }

    void SetName(const std::string& name)
    {
        Network::CopyToArray(name_, name);
    }

    virtual std::string ToString() const override
    {
        return std::string("Component name: ") + name_;
    }

private:
    char name_[Network::arraySize];
};
}  // namespace Network
