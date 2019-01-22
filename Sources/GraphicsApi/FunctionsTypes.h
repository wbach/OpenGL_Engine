#pragma once
#include "Types.h"
#include <unordered_map>
#include <functional>

namespace GraphicsApi
{
enum class GraphicFunctionType
{
    SHADER_SET_ID,
    SHADER_BIND_ATTRIBUTES,
    SHADER_VARIABLES_LOCATION,
    SHADER_CONNECT_TEXTURES
};
typedef std::unordered_map<GraphicFunctionType, std::function<void(uint32)>> GraphicsApiFunctions;
}  // GameEngine
