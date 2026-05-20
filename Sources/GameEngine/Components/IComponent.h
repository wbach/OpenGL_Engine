#pragma once
#include <Types.h>

#include <cstddef>
#include <functional>
#include <optional>
#include <string>

#include "ComponentType.h"
#include "FunctionType.h"
#include "Field.h"

class TreeNode;
namespace GameEngine
{
class GameObject;

namespace Components
{
typedef std::string ParamName;

class ENGINE_API IComponent
{
public:
    virtual ~IComponent()                                                     = default;
    virtual void Register()                                                   = 0;
    virtual void Deregister()                                                 = 0;
    virtual void CleanUp()                                                    = 0;
    virtual void Reload()                                                     = 0;
    virtual void ReqisterFunctions()                                          = 0;
    virtual bool IsActive() const                                             = 0;
    virtual void Activate()                                                   = 0;
    virtual void SetActive(bool)                                              = 0;
    virtual void Deactivate()                                                 = 0;
    virtual GameObject& GetParentGameObject()                                 = 0;
    virtual const GameObject& getParentGameObject() const                     = 0;
    virtual void readFromNode(const TreeNode&)                                = 0;
    virtual void writeToNode(TreeNode&) const                                 = 0;
    virtual std::optional<IdType> getRegisteredFunctionId(FunctionType) const = 0;
    virtual std::vector<FieldInfo> GetFields()                                = 0;
    virtual ComponentTypeID GetTypeId() const                                 = 0;
    virtual const std::string& GetTypeName() const                            = 0;
    virtual void SetTag(const std::string&)                                   = 0;
    virtual const std::string& GetTag() const                                 = 0;
};
}  // namespace Components
}  // namespace GameEngine
