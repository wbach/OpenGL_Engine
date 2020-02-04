#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "GameEngine/DebugTools/EditorInterface/MessageTypes.h"

namespace GameEngine
{
struct TransformMsg : public Network::IMessage
{
    TransformMsg()
        : IMessage(static_cast<uint8>(MessageTypes::Transform), Network::MessageTarget::Dedicated)
    {
    }
    TransformMsg(uint32 objectId, const vec3& pos, const vec3& rot, const vec3& scale)
        : IMessage(static_cast<uint8>(MessageTypes::Transform), Network::MessageTarget::Dedicated)
        , objectId_(objectId)
        , position_(pos)
        , rotation_(rot)
        , scale_(scale)
    {
    }

    uint32 objectId_;
    vec3 position_;
    vec3 rotation_;
    vec3 scale_;

    virtual std::string ToString() const override
    {
        return "objectId : " + std::to_string(objectId_);
    }
};
}  // namespace GameEngine
