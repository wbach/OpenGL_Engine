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
    void cameraUpdate();

protected:
    std::tuple<vec4, vec4, mat4> calculateLocalPosition() const;
    void updatePitchYaw(const MouseMoveEvent&);

private:
    void lockPitch();
    void updateYaw();
    vec2 calculateMouseMove(const vec2i&) const;

protected:
    Context& context;
    ThridPersonCameraComponent* thridPersonCameraComponent;
    vec3 relativeCamerePosition;
    float mouseSensitivity_;
    vec2 pitchLimit;
    vec2 rotationDir;
};
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
