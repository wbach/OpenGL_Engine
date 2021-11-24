#pragma once
#include "ShaderTypes.h"
#include "Utils/BidirectionalUnorderedMap.h"
#include <string>

namespace GraphicsApi
{
typedef std::unordered_map<GraphicsApi::ShaderType, std::string> ShadersFiles;
}  // namespace GraphicsApi
