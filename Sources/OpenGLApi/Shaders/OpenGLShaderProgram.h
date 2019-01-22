#pragma once
#include "Types.h"
#include <vector>

namespace OpenGLApi
{
struct OpenGLShaderProgram
{
    OpenGLShaderProgram(uint32 id, const std::string& name)
    : id(id)
    , name(name)
    {
    }
    uint32 id;
    std::string name;
    std::vector<uint32> shaderObjectsList;
};
} // GameEngine
