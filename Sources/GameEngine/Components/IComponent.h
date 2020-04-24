#pragma once
#include <string>
#include <unordered_map>

#include "ComponentsTypes.h"

namespace GameEngine
{
namespace Components
{
const std::string FILE{"file"};
const std::string STRING{"string"};
const std::string INT{"int"};
const std::string BOOL{"bool"};
const std::string FLOAT{"float"};
const std::string VECTOR{"vector"};

struct Param
{
    std::string type;
    std::string value;
};

typedef std::string ParamName;

class IComponent
{
public:
    virtual ~IComponent()                                                            = default;
    virtual void ReqisterFunctions()                                                 = 0;
    virtual ComponentsType GetType() const                                           = 0;
    virtual bool IsActive() const                                                    = 0;
    virtual void Activate()                                                          = 0;
    virtual void Deactivate()                                                        = 0;
    virtual void InitFromParams(const std::unordered_map<std::string, std::string>&) = 0;
    virtual std::unordered_map<ParamName, Param> GetParams() const                   = 0;
};
}  // namespace Components
}  // namespace GameEngine
