#pragma once
#include <Types.h>
#include <optional>

namespace GameEngine
{
namespace RayTracer
{
class StepPooler
{
public:
    StepPooler(const vec2ui& viewPort);
    std::optional<vec2ui> getNextStep();

private:
    vec2ui viewPort_;
    vec2ui current_;
    uint32_t progress_;
    uint32_t total_;
};
}  // namespace RayTracer
}  // namespace GameEngine
