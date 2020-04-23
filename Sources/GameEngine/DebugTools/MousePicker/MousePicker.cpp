#include "MousePicker.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
std::optional<float> SphereIntersect(const vec3& objectPosition, float radius, const vec3& rayDirection,
                                     const vec3& rayPosition)
{
    const auto& o = rayPosition;
    const auto& d = glm::normalize(rayDirection);
    const auto& c = objectPosition;
    auto v        = o - c;

    float A = glm::dot(d, d);
    float B = 2.f * glm::dot(v, d);
    float C = glm::dot(v, v) - radius * radius;

    float t0 = (-B - sqrt(B * B - 4 * A * C)) / (2 * A);
    float t1 = (-B + sqrt(B * B - 4 * A * C)) / (2 * A);

    if (t0 > 0 && t1 > 0)
    {
        if (t0 > t1)
            return t1;
        else
            return t0;
    }

    return {};
}

MousePicker::MousePicker(const CameraWrapper& camera, const Projection& projection, const vec2ui& windowSize)
    : camera_(camera)
    , projection_(projection)
    , windowSize_(windowSize)
{
}
GameObject* MousePicker::SelectObject(const vec2& mousePosition,
                                      const std::vector<std::unique_ptr<GameObject>>& objectList)
{
    return Intersect(objectList, CalculateMouseRay(mousePosition));
}
GameObject* MousePicker::Intersect(const std::vector<std::unique_ptr<GameObject>>& objectList, const vec3& ray)
{
    for (auto& object : objectList)
    {
        auto child = Intersect(object->GetChildren(), ray);

        if (child)
            return child;

        auto intersection = Intersect(*object, ray);
        if (intersection)
            return object.get();
    }
    return nullptr;
}
vec3 MousePicker::CalculateMouseRay(const vec2& mousePosition)
{
    vec4 clipCoords(mousePosition.x, mousePosition.y, -1.0f, 1.0f);
    auto eyeCoords = ConvertToEyeCoords(clipCoords);
    return ConvertToWorldCoords(eyeCoords);
}
vec4 MousePicker::ConvertToEyeCoords(const vec4& clipCoords)
{
    auto invertedMatrix = glm::inverse(projection_.GetProjectionMatrix());

    auto coords = invertedMatrix * clipCoords;
    return vec4(coords.x, coords.y, -1.0f, 0.0f);
}
vec3 MousePicker::ConvertToWorldCoords(const vec4& eyeCoords)
{
    auto invertedMatrix = glm::inverse(camera_.GetViewMatrix());

    auto coords = invertedMatrix * eyeCoords;
    return glm::normalize(Utils::Vec4ToVec3(coords));
}
std::optional<float> MousePicker::Intersect(const GameObject& object, const vec3& ray)
{
    auto radius = CalculateBoundingSphereRadius(object);
    return SphereIntersect(object.GetWorldTransform().GetPosition(), radius, ray, camera_.GetPosition());
}

float MousePicker::CalculateBoundingSphereRadius(const GameObject&)
{
    return 1.0f;
}

}  // namespace GameEngine
