#pragma once
#include <GLM/GLMUtils.h>
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include <optional>
#include "GameEngine/DebugTools/EditorInterface/MessageTypes.h"

namespace GameEngine
{
class ComponentParam : public Network::IMessage
{
public:
    ComponentParam()
        : IMessage(static_cast<uint8>(MessageTypes::ComponentParam), Network::MessageTarget::All)
    {
    }

    void SetName(const std::string& str)
    {
        Network::CopyToArray(name_, str);
    }

    void SetType(const std::string& str)
    {
        Network::CopyToArray(type_, str);
    }

    void SetValue(const std::string& str)
    {
        Network::CopyToArray(value_, str);
    }

    std::string GetName() const
    {
        return name_;
    }

    std::string GetType() const
    {
        return type_;
    }

    std::string GetValue() const
    {
        return value_;
    }

    virtual std::string ToString() const override
    {
        return std::string("param name: ") + name_;
    }

    uint32 gameObjectId_;

private:
    char componentName_[Network::arraySize];
    char name_[Network::arraySize];
    char value_[Network::arraySize];
    char type_[Network::arraySize];
};
}  // namespace GameEngine
