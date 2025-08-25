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
void regsiterComponentReadFunction(const ReadFunctions::ComponentName& name, ComponentReadFunction readFunction)
{
    ReadFunctions::instance().componentsReadFunctions.insert({name, readFunction});
}

const std::string CSTR_TYPE = "type";
}  // namespace Components
}  // namespace GameEngine
