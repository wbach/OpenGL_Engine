#include "MouseUtils.h"

#include <GLM/GLMUtils.h>
#include <Glm.h>
#include <Logger/Log.h>

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Renderers/Projection/IProjection.h"

namespace GameEngine
{
vec4 ConvertToEyeCoords(const IProjection& projection, const vec4& clipCoords)
{
    auto invertedMatrix = glm::inverse(projection.GetMatrix());
    auto coords         = invertedMatrix * clipCoords;
    return vec4(coords.x, coords.y, -1.0f, 0.0f);
}

vec3 ConvertToWorldCoords(const ICamera& camera, const vec4& eyeCoords)
{
    auto invertedMatrix = glm::inverse(camera.GetViewMatrix());
    auto coords         = invertedMatrix * eyeCoords;
    return glm::normalize(Utils::Vec4ToVec3(coords));
}

vec3 CalculateMouseRayDirection(const ICamera& camera, const vec2& mousePosition)
{
    vec4 clipCoords(mousePosition.x, mousePosition.y, -1.0f, 1.0f);
    auto eyeCoords = ConvertToEyeCoords(camera.GetProjection(), clipCoords);
    return ConvertToWorldCoords(camera, eyeCoords);
}
}  // namespace GameEngine
