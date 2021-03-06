#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct CameraMsg : public Network::IMessage
{
    CameraMsg()
    : IMessage(Convert(MessageTypes::CameraMsg), Network::MessageTarget::All)
    {
    }
    CameraMsg(
            const vec3& position,
            const vec3& rotation)
    : IMessage(Convert(MessageTypes::CameraMsg), Network::MessageTarget::All)
    , position{position}
    , rotation{rotation}
    {
    }

    vec3 position;
    vec3 rotation;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
