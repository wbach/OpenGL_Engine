#include "ComponentsReadFunctions.h"

#include <optional>

#include <Logger/Log.h>

namespace GameEngine
{
class GameObject;

namespace Components
{
ReadFunctions& ReadFunctions::instance()
{
    static ReadFunctions rf;
    return rf;
}

ComponentReadFunction ReadFunctions::get(const ComponentTypeName& name)
{
    if (auto id = getComponentTypeIdByName(name))
    {
        auto iter = componentsReadFunctions.find(*id);
        if (iter != componentsReadFunctions.end())
            return iter->second;
    }

    LOG_ERROR << "get component read function failed. Name=" << name;
    return nullptr;
}

bool ReadFunctions::isRegistered(const ComponentTypeName& name)
{
    return getComponentTypeIdByName(name).has_value();
}

void ReadFunctions::RegisterComponent(const ComponentType& type, ComponentReadFunction readFunction)
{
    componentsReadFunctions.insert({type.id, readFunction});
    typeNameToId.insert({type.name, type.id});
    LOG_DEBUG << "RegisterComponent: " << type;
}

void ReadFunctions::UnregisterComponent(const ComponentTypeName& name)
{
    if (auto id = getComponentTypeIdByName(name))
    {
        componentsReadFunctions.erase(*id);
        typeNameToId.erase(name);
    }
    else
    {
        LOG_ERROR << "UnregisterComponent failed. Name=" << name;
    }
}

std::optional<ComponentTypeID> ReadFunctions::getComponentTypeIdByName(const ComponentTypeName& name)
{
    auto iter = typeNameToId.find(name);
    if (iter != typeNameToId.end())
        return iter->second;

    LOG_ERROR << "UnregisterComponent failed. Name=" << name;
    return std::nullopt;
}

void regsiterComponentReadFunction(const ComponentType& type, ComponentReadFunction readFunction)
{
    ReadFunctions::instance().RegisterComponent(type, readFunction);
}

void unregsiterComponentReadFunction(const ComponentTypeName& name)
{
    ReadFunctions::instance().UnregisterComponent(name);
}

std::optional<ComponentTypeID> getComponentTypeIdByName(const ComponentTypeName& name)
{
    return ReadFunctions::instance().getComponentTypeIdByName(name);
}

const std::string CSTR_TYPE = "type";
}  // namespace Components
}  // namespace GameEngine
