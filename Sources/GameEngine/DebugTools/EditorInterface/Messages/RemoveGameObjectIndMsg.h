#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "GameEngine/DebugTools/EditorInterface/MessageTypes.h"

namespace GameEngine
{
struct RemoveGameObjectIndMsg : public Network::IMessage
{
public:
    RemoveGameObjectIndMsg()
        : RemoveGameObjectIndMsg(0)
    {
    }
    RemoveGameObjectIndMsg(uint32 id)
        : IMessage(static_cast<uint8>(MessageTypes::RemoveGameObjectInd), Network::MessageTarget::All)
        , id_(id)
    {
    }

    uint32 id_ = 0;

    virtual std::string ToString() const override
    {
        return "ID: " + std::to_string(id_);
    }
};
}  // namespace GameEngine
