#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "GameEngine/DebugTools/EditorInterface/MessageTypes.h"

namespace GameEngine
{
struct NewGameObjectIndMsg : public Network::IMessage
{
public:
    NewGameObjectIndMsg()
        : NewGameObjectIndMsg("")
    {

    }
    NewGameObjectIndMsg(const std::string& name)
        : IMessage(static_cast<uint8>(MessageTypes::NewGameObjectInd), Network::MessageTarget::All)
    {
        Network::CopyToArray(name_, name);
    }
    std::string GetName() const
    {
        return name_;
    }

    uint32 id = 0;
    uint32 parentId = 0;

    virtual std::string ToString() const override
    {
        return "ID: " + std::to_string(id) + "\nName: " + name_ + "\nParentId: " + std::to_string(parentId);
    }

private:
    char name_[Network::arraySize];
};
}  // namespace Network
