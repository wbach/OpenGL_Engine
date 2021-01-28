#include "ComponentsReadFunctions.h"

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
void regsiterComponentReadFunction(const std::string& componentName, ComponentReadFunction readFunction)
{
    ReadFunctions::instance().componentsReadFunctions.insert({componentName, readFunction});
}

const std::string CSTR_TYPE = "type";
}  // namespace Components
}  // namespace GameEngine
