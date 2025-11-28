#include "StaticRayTracer.h"

#include <Mutex.hpp>
#include <Thread.hpp>
#include <chrono>
#include <iostream>

#include "Camera.h"
#include "GameEngine/Components/Lights/DirectionalLightComponent.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Projection/PerspectiveProjection.h"
#include "GameEngine/Resources/Models/Material.h"
#include "Intersection.h"
#include "Objects/IObject.h"
#include "Objects/Triangle.h"
#include "Ray.h"
#include "Scene.h"
#include "StepPooler.h"

namespace GameEngine
{
namespace RayTracer
{
namespace
{
std::unique_ptr<StepPooler> stepPooler;
std::mutex stepMutex_;
}  // namespace

RayTracerEngine::RayTracerEngine(RendererContext& context, uint32 threadsCount)
    : rendererContext_{context}
    , threadsCount_{threadsCount}
{
}

RayTracerEngine::~RayTracerEngine()
{
}

void RayTracerEngine::subscribe(GameObject& gameObject)
{
    //    auto iter = std::find_if(subscribes_.begin(), subscribes_.end(),
    //                             [id = gameObject.GetId()](const auto &sub) { return sub.gameObject->GetId() == id;
    //                             });

    auto iter = subscribers_.find(gameObject.GetId());

    if (iter != subscribers_.end())
        return;

    auto rendererComponent = gameObject.GetComponent<Components::RendererComponent>();

    if (rendererComponent == nullptr)
        return;

    auto model = rendererComponent->GetModelWrapper().Get(LevelOfDetail::L1);
    if (not model or model->GetMeshes().empty())
        return;

    auto& sub = subscribers_[gameObject.GetId()];

    for (const auto& mesh : model->GetMeshes())
    {
        const auto& data = mesh.GetCMeshDataRef();

        for (size_t i = 0; i < data.indices_.size(); i += 3)
        {
            auto i1 = 3 * static_cast<size_t>(data.indices_[i]);
            auto i2 = 3 * static_cast<size_t>(data.indices_[i + 1]);
            auto i3 = 3 * static_cast<size_t>(data.indices_[i + 2]);

            vec3 v0(data.positions_[i1], data.positions_[i1 + 1], data.positions_[i1 + 2]);
            vec3 v1(data.positions_[i2], data.positions_[i2 + 1], data.positions_[i2 + 2]);
            vec3 v2(data.positions_[i3], data.positions_[i3 + 1], data.positions_[i3 + 2]);

            auto triangle = std::make_unique<Triangle>(gameObject.GetWorldTransform(), mesh.GetMaterial(), v0, v1, v2);
            sub.push_back(triangle->getId());
            scene_.objects_.push_back(std::move(triangle));
        }
    }
}

void RayTracerEngine::unSubscribe(GameObject& gameObject)
{
    auto iter = subscribers_.find(gameObject.GetId());
    if (iter != subscribers_.end())
    {
        for (auto id : iter->second)
        {
            auto iter = std::find_if(scene_.objects_.begin(), scene_.objects_.end(),
                                     [id](const auto& obj) { return id == obj->getId(); });
            // if (iter != scene_.objects_.end())
            scene_.objects_.erase(iter);
        }
    }
}

void RayTracerEngine::render(const vec2ui& size, float, const File& outputFile)
{
    stepPooler.reset(new StepPooler(scene_.camera_->getViewPort()));

    image_ = Utils::Image();

    image_.width  = size.x;
    image_.height = size.y;
    image_.setChannels(4);
    image_.allocateImage<uint8>();

    if (auto projection = dynamic_cast<const PerspectiveProjection*>(&rendererContext_.camera_->GetProjection()))
    {
        scene_.camera_ = std::make_unique<RayTracer::Camera>(size, glm::radians(projection->GetFoV()), vec3(0, 1, 0),
                                                             rendererContext_.camera_->GetPosition(),
                                                             rendererContext_.camera_->GetDirection());

        multiThreadsRun(threadsCount_);

        Utils::SaveImage(image_, outputFile.GetAbsolutePath().string());
    }
}

void RayTracerEngine::multiThreadsRun(uint32 threadsCount)
{
    auto startTime = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;

    for (uint32 x = 0; x < threadsCount - 1; ++x)
    {
        threads.emplace_back(&RayTracerEngine::run, this);
    }

    run();

    for (auto& thread : threads)
    {
        thread.join();
    }

    std::cout << std::endl;

    auto endTime                                              = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::ratio<1, 1>> durration = endTime - startTime;

    std::cout << "Time : " << durration.count() << "s." << std::endl;
}

void RayTracerEngine::run()
{
    while (true)
    {
        std::optional<vec2ui> s;
        {
            std::lock_guard<std::mutex> m(stepMutex_);
            s = stepPooler->getNextStep();
        }

        if (!s)
        {
            break;
        }

        step(s->x, s->y);
    }
}

void RayTracerEngine::step(uint32 x, uint32 y)
{
    float energy = 1.0f;
    vec3 pixelVector(x, y, 1.0f);
    vec3 dirVector = glm::normalize(pixelVector * scene_.camera_->getMatrix());
    Ray ray(scene_.camera_->getPosition(), dirVector, x, y);
    auto color = trace(ray, energy);

    image_.setPixel(vec2ui(x, y), Color(color));
}

vec3 RayTracerEngine::trace(const Ray& ray, float& energy, const IObject* parent)
{
    auto intersection = findIntersection(ray, parent);

    if (!intersection)
    {
        return vec3(0.f);
    }

    auto color = calculateColor(ray, intersection, energy);

    const auto& material = intersection.getObject()->getMaterial();
    // energy -= material.absorption_;
    energy -= (1.f - material.reflectivity);

    auto isReflect = material.reflectivity > std::numeric_limits<float>::epsilon();

    if (energy < 0.1f || !isReflect)
    {
        return color;
    }

    const auto& normal         = intersection.getNormal();
    auto reflectedRayDirection = ray.getDirection() - (2.f * ray.getDirection() * normal) * normal;
    Ray reflectedRay(intersection.getPoint(), reflectedRayDirection, ray.getX(), ray.getY());

    auto totalColor = color + trace(reflectedRay, energy, intersection.getObject());
    limtColorValue(totalColor);
    return totalColor;
}
Intersection RayTracerEngine::findIntersection(const Ray& ray, const IObject* parent) const
{
    Intersection result;

    float t = std::numeric_limits<float>::max();

    for (const auto& obj : scene_.objects_)
    {
        if (parent && obj->getId() == parent->getId())
        {
            continue;
        }

        auto intersectionPoint = obj->intersect(ray);

        if (intersectionPoint)
        {
            if (*intersectionPoint < t)
            {
                t = *intersectionPoint;
                result.setObject(obj.get());
            }
        }
    }

    result.calculateIntersectionPointAndObjNormal(ray, t);

    return result;
}
vec3 RayTracerEngine::calculateColor(const Ray& ray, const Intersection& intersection, float energy)
{
    vec3 outputColor = scene_.bgColor_;

    // const auto &normal = intersection.getNormal();

    auto directionalLights =
        rendererContext_.scene_->getComponentController().GetAllActiveComponentsOfType<Components::DirectionalLightComponent>();
    for (const auto& light :directionalLights)
    {
        outputColor +=
            procesLight(ray, *light, intersection.getPoint(), intersection.getNormal(), intersection.getObject()) * energy;
    }

    limtColorValue(outputColor);
    return outputColor;
}
vec3 RayTracerEngine::procesLight(const Ray& ray, const Components::DirectionalLightComponent& light, const vec3& intersectionPoint, const vec3& normal,
                                  const IObject* obj)
{
    auto lightDirection = glm::normalize(light.getParentGameObject().GetWorldTransform().GetPosition() - intersectionPoint);
    Ray shadowRay(intersectionPoint, lightDirection, ray.getX(), ray.getY());

    if (isShadow(shadowRay))
    {
        return vec3(0);
    }
    vec3 ambient = obj->getMaterial().ambient.xyz() * light.color.xyz();

    float intensity = glm::dot(normal, lightDirection);

    if (intensity < 0)
    {
        return ambient;
    }

    vec3 r  = 2.f * intensity * normal - lightDirection;
    vec3 v  = glm::normalize(ray.getPosition() - intersectionPoint);
    float s = glm::dot(v, r);

    vec3 diffuse = obj->getMaterial().diffuse.xyz() * intensity;
    vec3 specular(0.f);

    if (s > 0.f && s < 90.f * M_PI / 180.f)
    {
        specular = obj->getMaterial().specular.xyz() * light.color.xyz() * powf(s, obj->getMaterial().shineDamper);
    }

    return ambient + diffuse + specular;
}

bool RayTracerEngine::isShadow(const Ray& ray) const
{
    for (const auto& obj : scene_.objects_)
    {
        const auto& intersectionPoint = obj->intersect(ray);

        if (intersectionPoint)
        {
            return true;
        }
    }

    return false;
}
void RayTracerEngine::limtColorValue(vec3& color) const
{
    if (color.x > 1.f)
    {
        color.x = 1.f;
    }

    if (color.y > 1.f)
    {
        color.y = 1.f;
    }

    if (color.z > 1.f)
    {
        color.z = 1.f;
    }
}
}  // namespace RayTracer
}  // namespace GameEngine
