#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"
#include "Param.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct ComponentDataMessage : public Network::IMessage
{
    ComponentDataMessage()
    : IMessage(Convert(MessageTypes::ComponentDataMessage), Network::MessageTarget::All)
    {
    }
    ComponentDataMessage(
            const std::string& name,
            uint8 gameObjectId,
            const std::vector<Param>& params,
            const vec3& testVec3Param)
    : IMessage(Convert(MessageTypes::ComponentDataMessage), Network::MessageTarget::All)
    , name{name}
    , gameObjectId{gameObjectId}
    , params{params}
    , testVec3Param{testVec3Param}
    {
    }

    std::string name;
    uint8 gameObjectId;
    std::vector<Param> params;
    vec3 testVec3Param;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
