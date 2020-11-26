#include "MouseUtils.h"
#include <GLM/GLMUtils.h>
#include <Glm.h>
#include "GameEngine/Camera/CameraWrapper.h"
#include "GameEngine/Renderers/Projection.h"

namespace GameEngine
{
vec4 ConvertToEyeCoords(const Projection& projection, const vec4& clipCoords)
{
    auto invertedMatrix = glm::inverse(projection.GetProjectionMatrix());
    auto coords = invertedMatrix * clipCoords;
    return vec4(coords.x, coords.y, -1.0f, 0.0f);
}

vec3 ConvertToWorldCoords(const CameraWrapper& camera, const vec4& eyeCoords)
{
    auto invertedMatrix = glm::inverse(camera.GetViewMatrix());
    auto coords = invertedMatrix * eyeCoords;
    return glm::normalize(Utils::Vec4ToVec3(coords));
}

vec3 CalculateMouseRayDirection(const Projection& projection, const CameraWrapper& camera, const vec2& mousePosition)
{
    vec4 clipCoords(mousePosition.x, mousePosition.y, -1.0f, 1.0f);
    auto eyeCoords = ConvertToEyeCoords(projection, clipCoords);
    return ConvertToWorldCoords(camera, eyeCoords);
}
}  // namespace GameEngine
