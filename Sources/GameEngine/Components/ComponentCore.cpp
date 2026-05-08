#include "ComponentCore.h"

#include <Logger/Log.h>
#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/IComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "Utils.h"

namespace GameEngine
{
namespace Components
{
ComponentCore::ComponentCore(const ComponentType& type, ComponentContext& componentContext, GameObject& gameObject)
    : type_(type)
    , thisObject_(gameObject)
    , componentContext_(componentContext)
    , isActive_(true)
    , componentRegistredId_(0)
{
    LOG_DEBUG << "type = " << type_;
}

ComponentCore::ComponentCore(uint32 id, const char* name, ComponentContext& componentContext, GameObject& gameObject)
    : type_({.id = id, .name = std::string(name)})
    , thisObject_(gameObject)
    , componentContext_(componentContext)
    , isActive_(true)
    , componentRegistredId_(0)
{
}

ComponentCore::~ComponentCore()
{
}

void ComponentCore::CleanUp()
{
}

void ComponentCore::ReqisterFunctions()
{
}

void ComponentCore::Reload()
{
}

void ComponentCore::Register()
{
    componentRegistredId_ = componentContext_.componentController_.RegisterComponent(type_.id, this);
}
void ComponentCore::Deregister()
{
    UnregisterFunctions();
}
void ComponentCore::RegisterFunction(FunctionType type, std::function<void()> func, const Dependencies& dependencies)
{
    registeredFunctionsIds_.insert(
        {componentContext_.componentController_.RegisterFunction(thisObject_.GetId(), type_, type, func, dependencies), type});
}
void ComponentCore::UnregisterFunctions()
{
    for (const auto& [id, type] : registeredFunctionsIds_)
    {
        componentContext_.componentController_.UnRegisterFunction(thisObject_.GetId(), type, id);
    }

    if (componentRegistredId_)
    {
        componentContext_.componentController_.UnRegisterComponent(type_.id, *componentRegistredId_);
        componentRegistredId_.reset();
    }
    else
    {
        LOG_ERROR << "componentRegistredId not set!";
    }
}
ComponentTypeID ComponentCore::GetTypeId() const
{
    return type_.id;
}
void ComponentCore::Activate()
{
    isActive_ = true;
    changeActivateStateRegisteredFunctions();
}
void ComponentCore::Deactivate()
{
    isActive_ = false;
    changeActivateStateRegisteredFunctions();
}
void ComponentCore::SetActive(bool is)
{
    is ? Activate() : Deactivate();
}
GameObject& ComponentCore::GetParentGameObject()
{
    return thisObject_;
}
const GameObject& ComponentCore::getParentGameObject() const
{
    return thisObject_;
}
std::optional<IdType> ComponentCore::getRegisteredFunctionId(FunctionType functionType) const
{
    for (const auto& [id, type] : registeredFunctionsIds_)
    {
        if (type == functionType)
            return id;
    }
    return std::nullopt;
}

std::vector<FieldInfo> ComponentCore::GetFields()
{
    return {};
}

const std::string& ComponentCore::GetTypeName() const
{
    return type_.name;
}
bool ComponentCore::IsActive() const
{
    return isActive_;
}

void ComponentCore::changeActivateStateRegisteredFunctions()
{
    for (const auto& [id, type] : registeredFunctionsIds_)
    {
        componentContext_.componentController_.setActivateStateOfComponentFunction(thisObject_.GetId(), type, id, isActive_);
    }
}
const std::string& ComponentCore::GetTag() const
{
    return tag;
}
void ComponentCore::SetTag(const std::string& t)
{
    tag = t;
}
void ComponentCore::readFromNode(const TreeNode& node)
{
    auto activeStr = node.getAttributeValue(CSTR_ACTIVE);
    if (not activeStr.empty())
    {
        SetActive(Utils::StringToBool(activeStr));
    }

    tag = node.getAttributeValue(CSTR_TAG);
    this->read(node);
}
void ComponentCore::writeToNode(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    node.attributes_.insert({CSTR_ACTIVE, Utils::BoolToString(IsActive())});
    node.attributes_.insert({CSTR_TAG, tag});
    this->write(node);
}
void ComponentCore::read(const TreeNode& node)
{
}
void ComponentCore::write(TreeNode& node) const
{
}
}  // namespace Components
}  // namespace GameEngine
