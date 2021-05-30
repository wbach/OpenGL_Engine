#pragma once
#include <Types.h>
#include <Utils/Image/Image.h>
#include <Utils/Image/ImageUtils.h>
#include <memory>
#include "GameEngine/Resources/File.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Scene.h"

namespace GameEngine
{
class Light;
class GameObject;
struct RendererContext;

namespace RayTracer
{
class Ray;
class IImage;
class IObject;
class Intersection;

class RayTracerEngine
{
public:
    RayTracerEngine(RendererContext&, uint32 threadsCount = std::thread::hardware_concurrency());
    ~RayTracerEngine();

    void subscribe(GameObject&);
    void unSubscribe(GameObject&);
    void render(const vec2ui&, float, const GameEngine::File&);

private:
    void run();
    void step(uint32 x, uint32 y);
    void multiThreadsRun(uint32 threadsCount);
    vec3 trace(const Ray& ray, float& energy, const IObject* parent = nullptr);
    Intersection findIntersection(const Ray& ray, const IObject* parent) const;
    vec3 calculateColor(const Ray& ray, const Intersection& intersection, float energy);
    vec3 procesLight(const Ray& ray, const Light& light, const vec3& intersectionPoint, const vec3& normal,
                     const IObject* obj);
    bool isShadow(const Ray& ray) const;
    void limtColorValue(vec3& color) const;

private:
    RendererContext& rendererContext_;
    RayTracer::Scene scene_;
    Utils::Image image_;
    uint32 threadsCount_;

    std::unordered_map<IdType, std::vector<IdType>> subscribers_;
};
}  // namespace RayTracer
}  // namespace GameEngine
