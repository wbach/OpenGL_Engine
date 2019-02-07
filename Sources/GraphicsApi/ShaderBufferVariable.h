#pragma once
#include "VariableType.h"
#include <string>

namespace GraphicsApi
{
struct ShaderBufferVariable
{
    VariableType type;
    std::string name;
};
}  // namespace GraphicsApi
