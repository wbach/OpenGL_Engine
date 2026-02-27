#pragma once
#include <Types.h>

#include "../ConfigurationParam.h"

namespace GameEngine
{
namespace Params
{
struct ENGINE_API Window
{
    Window();

    ConfigurationParam<vec2ui> size;
    ConfigurationParam<bool> fullScreen;
    ConfigurationParam<std::string> name;
};
}  // namespace Params
}  // namespace GameEngine