#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include <optional>
#include "GameEngine/DebugTools/EditorInterface/MessageTypes.h"

namespace GameEngine
{
struct CameraMsg : public Network::IMessage
{
public:
    CameraMsg()
        : IMessage(static_cast<uint8>(MessageTypes::CameraMsg), Network::MessageTarget::All)
    {
    }

    vec3 position_;
    vec3 rotation_;

    virtual std::string ToString() const override
    {
        return std::string("camera name: ");
    }
};
}  // namespace GameEngine
