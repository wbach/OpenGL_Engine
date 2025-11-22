#include "MousePicker.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/DebugTools/Common/MouseUtils.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
std::optional<float> SphereIntersect(const MousePicker::Ray& ray, const vec3& objectPosition, float radius)
{
    const auto& o = ray.position;
    const auto& d = glm::normalize(ray.direction);
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

std::optional<float> BoundingBoxIntersect(const MousePicker::Ray& ray, const BoundingBox& box)
{
    const auto& min = box.min();
    const auto& max = box.max();

    float tmin = -std::numeric_limits<float>::infinity();
    float tmax = std::numeric_limits<float>::infinity();

    auto checkAxis = [&](float rayOrigin, float rayDir, float boxMin, float boxMax) -> bool
    {
        float t1, t2;
        if (std::fabs(rayDir) < 1e-8f)
        {
            if (rayOrigin < boxMin || rayOrigin > boxMax)
                return false;
            t1 = -std::numeric_limits<float>::infinity();
            t2 = std::numeric_limits<float>::infinity();
        }
        else
        {
            t1 = (boxMin - rayOrigin) / rayDir;
            t2 = (boxMax - rayOrigin) / rayDir;
            if (t1 > t2)
                std::swap(t1, t2);
        }

        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);

        return tmax >= tmin;
    };

    if (!checkAxis(ray.position.x, ray.direction.x, min.x, max.x))
        return std::nullopt;
    if (!checkAxis(ray.position.y, ray.direction.y, min.y, max.y))
        return std::nullopt;
    if (!checkAxis(ray.position.z, ray.direction.z, min.z, max.z))
        return std::nullopt;

    return tmin;
}

MousePicker::MousePicker(const ICamera& camera)
    : camera_(camera)
{
}
GameObject* MousePicker::SelectObject(const vec2& mousePosition, const std::vector<std::unique_ptr<GameObject>>& objectList)
{
    Ray ray{camera_.GetPosition(), CalculateMouseRayDirection(camera_, mousePosition)};
    return Intersect(objectList, ray);
}

std::optional<std::pair<GameObject*, float>> MousePicker::IntersectObject(const GameObject* object, const MousePicker::Ray& ray)
{
    std::optional<std::pair<GameObject*, float>> closest;

    auto renderComponent = object->GetComponent<Components::RendererComponent>();
    if (renderComponent)
    {
        auto model       = renderComponent->GetModelWrapper().Get(LevelOfDetail::L1);
        auto boundingBox = model->getBoundingBox();
        boundingBox.scale(object->GetWorldTransform().GetScale());
        boundingBox.translate(object->GetWorldTransform().GetPosition());

        if (auto t = BoundingBoxIntersect(ray, boundingBox))
        {
            closest = std::make_pair(const_cast<GameObject*>(object), *t);
        }
    }

    for (const auto& child : object->GetChildren())
    {
        if (auto childClosest = IntersectObject(child.get(), ray))
        {
            if (!closest || childClosest->second < closest->second)
            {
                closest = childClosest;
            }
        }
    }

    return closest;
}

GameObject* MousePicker::Intersect(const std::vector<std::unique_ptr<GameObject>>& objectList, const MousePicker::Ray& ray)
{
    GameObject* closestObject = nullptr;
    float closestT            = std::numeric_limits<float>::infinity();

    for (const auto& object : objectList)
    {
        if (auto objClosest = IntersectObject(object.get(), ray))
        {
            if (objClosest->second < closestT)
            {
                closestT      = objClosest->second;
                closestObject = objClosest->first;
            }
        }
    }

    return closestObject;
}

std::optional<float> MousePicker::Intersect(const GameObject& object, const MousePicker::Ray& ray)
{
    auto radius = CalculateBoundingSphereRadius(object);
    return SphereIntersect(ray, object.GetWorldTransform().GetPosition(), radius);
}

float MousePicker::CalculateBoundingSphereRadius(const GameObject& gameObject)
{
    auto max = glm::compMax(gameObject.GetLocalTransform().GetScale());
    return max / 2.f;
}

}  // namespace GameEngine
