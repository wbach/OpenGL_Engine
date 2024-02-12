#pragma once
#include <Utils/Fsm/Actions.h>
#include <tuple>
#include "../Context.h"
#include "../ThridPersonCameraEvents.h"

namespace GameEngine
{
namespace Components
{
class ThridPersonCameraComponent;
namespace Camera
{
class StateBase
{
public:
    StateBase(Context&);
    StateBase(Context&, const vec3&);

    void onEnter();
    void setUpdateFunc();
    virtual void update() = 0;
    void pushEventToQueue(const Camera::Event&) const;

protected:
    void cameraUpdate(float, float);
    std::tuple<vec4, vec4, mat4> calculateLocalPosition() const;

protected:
    Context& context;
    ThridPersonCameraComponent* thridPersonCameraComponent;
    vec3 relativeCamerePosition;
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
