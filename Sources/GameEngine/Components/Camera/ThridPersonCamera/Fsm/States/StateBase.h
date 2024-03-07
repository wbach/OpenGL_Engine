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
    virtual void update();
    void pushEventToQueue(const Camera::Event&) const;

protected:
    void cameraUpdate();
    std::tuple<vec4, vec4, mat4> calculateLocalPosition() const;

private:
    void lockPitch();
    void updateYaw();
    vec2 calculateMouseMove() const;
    void updatePitchYaw(const vec2&);

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
