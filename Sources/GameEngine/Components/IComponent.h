#pragma once
#include "ComponentsTypes.h"

namespace GameEngine
{
namespace Components
{
class IComponent
{
public:
    virtual ~IComponent()                  = default;
    virtual void ReqisterFunctions()       = 0;
    virtual ComponentsType GetType() const = 0;
    virtual bool IsActive() const          = 0;
    virtual void Activate()                = 0;
    virtual void Deactivate()              = 0;
};
}  // namespace Components
}  // namespace GameEngine
