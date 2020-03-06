#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct Transform : public Network::IMessage
{
    Transform()
    : IMessage(Convert(MessageTypes::Transform), Network::MessageTarget::All)
    {
    }
    Transform(
            uint32 objectId,
            const vec3& position,
            const vec3& rotation,
            const vec3& scale)
    : IMessage(Convert(MessageTypes::Transform), Network::MessageTarget::All)
    , objectId{objectId}
    , position{position}
    , rotation{rotation}
    , scale{scale}
    {
    }

    uint32 objectId;
    vec3 position;
    vec3 rotation;
    vec3 scale;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
