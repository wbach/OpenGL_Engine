#pragma once
#include <vector>
#include <memory>
#include <Types.h>
#include "Camera.h"
#include "Objects/IObject.h"
#include "GameEngine/Lights/Light.h"

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
