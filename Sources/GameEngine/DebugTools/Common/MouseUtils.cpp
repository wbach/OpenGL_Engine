#include "MouseUtils.h"

#include <GLM/GLMUtils.h>
#include <Glm.h>
#include <Logger/Log.h>

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Renderers/Projection/IProjection.h"
#include "GameEngine/Renderers/Projection/PerspectiveProjection.h"
#include "glm/geometric.hpp"

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

Ray CalculateMouseRayForPerspectiveCamera(const ICamera& camera, const vec2& mousePosition)
{
    vec4 clipCoords(mousePosition.x, mousePosition.y, -1.0f, 1.0f);
    auto eyeCoords = ConvertToEyeCoords(camera.GetProjection(), clipCoords);
    auto dir       = ConvertToWorldCoords(camera, eyeCoords);
    return Ray{.position = camera.GetPosition(), .direction = dir};
}
Ray CalculateMouseRayForOrthoCamera(const ICamera& cam, const vec2& mouse)
{
    vec4 ndc(mouse.x, mouse.y, -1.0f, 1.0f);
    vec4 viewPos   = glm::inverse(cam.GetProjection().GetMatrix()) * ndc;
    vec4 worldPos4 = glm::inverse(cam.GetViewMatrix()) * viewPos;
    vec3 worldPos  = glm::vec3(worldPos4) / worldPos4.w;

    return Ray{.position = worldPos, .direction = glm::normalize(cam.GetDirection())};
}
Ray CalculateMouseRay(const ICamera& camera, const vec2& mouse)
{
    if (camera.GetProjection().GetType() == ProjectionType::Perspective)
    {
        return CalculateMouseRayForPerspectiveCamera(camera, mouse);
    }
    else
    {
        return CalculateMouseRayForOrthoCamera(camera, mouse);
    }
}
}  // namespace GameEngine
