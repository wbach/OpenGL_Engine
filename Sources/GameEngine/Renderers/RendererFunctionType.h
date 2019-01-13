#pragma once
#include <vector>
#include <functional>
#include <unordered_map>

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
typedef std::function<void(Scene*)> RendererFunction;
typedef std::unordered_map<RendererFunctionType, std::vector<RendererFunction>> RendererFunctions;
}  // GameEngine
