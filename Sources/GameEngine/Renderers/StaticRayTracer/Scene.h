#pragma once
#include <Types.h>

#include <memory>
#include <vector>

#include "Camera.h"
#include "GameEngine/Lights/Light.h"
#include "Objects/IObject.h"

namespace GameEngine
{
namespace RayTracer
{
struct Scene
{
    vec3 bgColor_;
    std::unique_ptr<Camera> camera_;
    std::vector<std::unique_ptr<IObject>> objects_;
};
}  // namespace RayTracer
}  // namespace GameEngine
