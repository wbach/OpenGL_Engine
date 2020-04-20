#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "MessageTypes.h"

namespace GameEngine
{
namespace DebugNetworkInterface
{
struct NewLight : public Network::IMessage
{
    NewLight()
    : IMessage(Convert(MessageTypes::NewLight), Network::MessageTarget::All)
    {
    }
    NewLight(
            uint32 id,
            uint32 type,
            const vec3& position,
            const vec3& colour,
            const vec3& attenuation,
            float cutOff,
            const vec3& direction)
    : IMessage(Convert(MessageTypes::NewLight), Network::MessageTarget::All)
    , id{id}
    , type{type}
    , position{position}
    , colour{colour}
    , attenuation{attenuation}
    , cutOff{cutOff}
    , direction{direction}
    {
    }

    uint32 id;
    uint32 type;
    vec3 position;
    vec3 colour;
    vec3 attenuation;
    float cutOff;
    vec3 direction;
};
} // namespace DebugNetworkInterface
} // namespace GameEngine
