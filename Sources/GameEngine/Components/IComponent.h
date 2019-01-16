
#pragma once
#include "ComponentsTypes.h"

namespace GameEngine
{
namespace Components
{
class IComponent
{
public:
    virtual ~IComponent() {}
    virtual void ReqisterFunctions() = 0;
    virtual ComponentsType GetType() = 0;
};
}  // namespace Components
}  // namespace GameEngine
