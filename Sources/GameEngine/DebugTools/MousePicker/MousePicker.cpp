#include "MousePicker.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>

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

void swap(float& a, float& b)
{
    float tmp = a;
    a         = b;
    b         = tmp;
}

bool BoundingBoxIntersect(const MousePicker::Ray& ray, const BoundingBox& boundingBox)
{
    const auto& min = boundingBox.min();
    const auto& max = boundingBox.max();

    float tmin = (min.x - ray.position.x) / ray.direction.x;
    float tmax = (max.x - ray.position.x) / ray.direction.x;

    if (tmin > tmax)
        swap(tmin, tmax);

    float tymin = (min.y - ray.position.y) / ray.direction.y;
    float tymax = (max.y - ray.position.y) / ray.direction.y;

    if (tymin > tymax)
        swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;

    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (min.z - ray.position.z) / ray.direction.z;
    float tzmax = (max.z - ray.position.z) / ray.direction.z;

    if (tzmin > tzmax)
        swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    if (tzmin > tmin)
        tmin = tzmin;

    if (tzmax < tmax)
        tmax = tzmax;

    return true;
}

MousePicker::MousePicker(const CameraWrapper& camera, const Projection& projection)
    : camera_(camera)
    , projection_(projection)
{
}
GameObject* MousePicker::SelectObject(const vec2& mousePosition,
                                      const std::vector<std::unique_ptr<GameObject>>& objectList)
{
    Ray ray{camera_.GetPosition(), CalculateMouseRayDirection(projection_, camera_, mousePosition)};
    return Intersect(objectList, ray);
}
GameObject* MousePicker::Intersect(const std::vector<std::unique_ptr<GameObject>>& objectList,
                                   const MousePicker::Ray& ray)
{
    for (auto& object : objectList)
    {
        auto child = Intersect(object->GetChildren(), ray);

        if (child)
            return child;

        auto renderComponent = object->GetComponent<Components::RendererComponent>();

        if (not renderComponent)
            continue;

        //auto model = renderComponent->GetModelWrapper().Get(LevelOfDetail::L1);
        //auto localBoundingBox = model->getBoundingBox();
        //BoundingBox worldBoundingBox;
        //worldBoundingBox.min(object->GetWorldTransform().GetMatrix() * vec4(localBoundingBox.min(), 1.f));
        //worldBoundingBox.max(object->GetWorldTransform().GetMatrix() * vec4(localBoundingBox.max(), 1.f));

        //auto intersection = BoundingBoxIntersect(ray, worldBoundingBox);
        auto intersection = Intersect(*object, ray);
        if (intersection)
            return object.get();
    }
    return nullptr;
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
