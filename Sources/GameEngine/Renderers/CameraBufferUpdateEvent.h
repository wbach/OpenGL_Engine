#pragma once
#include <Types.h>

#include <functional>

#include "GameEngine/Camera/ICamera.h"
#include <Logger/Log.h>

namespace GameEngine
{
class CameraBufferUpdateEvent
{
public:
    CameraBufferUpdateEvent(ICamera& camera)
        : camera(camera)
    {
    }

    void Execute()
    {
        camera.get().Update();
        camera.get().UpdatePerFrameBuffer();
    }

private:
    std::reference_wrapper<ICamera> camera;
};
}  // namespace GameEngine
