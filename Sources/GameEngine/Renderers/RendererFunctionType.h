#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include "GameEngine/Time/Time.h"

namespace GameEngine
{
enum class RendererFunctionType
{
    PRERENDER,
    PRECONFIGURE,
    CONFIGURE,
    UPDATE,
    POSTUPDATE,
    ONENDFRAME
};
class Scene;
typedef std::function<void(const Scene&, const Time&)> RendererFunction;
typedef std::unordered_map<RendererFunctionType, std::vector<RendererFunction>> RendererFunctions;
}  // namespace GameEngine
