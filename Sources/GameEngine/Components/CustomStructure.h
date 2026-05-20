#pragma once
#include "Field.h"

namespace GameEngine
{
namespace Components
{
struct CustomStructure
{
    using Fields = std::vector<FieldInfo>;
    std::string name;
    Fields fields;
};
}  // namespace Components
}  // namespace GameEngine
