#pragma once
#include <Types.h>
#include "RendererParams/WaterParams/WaterType.h"
#include <GraphicsApi/RendererType.h>
#include <Logger/Log.h>

namespace GameEngine
{
namespace Params
{
std::string paramToString(int);
std::string paramToString(uint32);
std::string paramToString(bool);
std::string paramToString(float);
std::string paramToString(const vec2ui&);
std::string paramToString(const std::string&);
std::string paramToString(GraphicsApi::RendererType);
std::string paramToString(LogginLvl);
LogginLvl paramFromString(const std::string&);
}  // namespace Params
}  // namespace GameEngine
