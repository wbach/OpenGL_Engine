#pragma once
#include <Utils/Mutex.hpp>
#include <functional>
#include "Camera.h"

namespace GameEngine
{
class CustomCamera : public Camera
{
public:
    void setOnUpdate(std::function<void()> func)
    {
        std::lock_guard<std::mutex> lk(onUpdateMutex);
        onUpdate = func;
    }

    void Update() override
    {
        std::lock_guard<std::mutex> lk(onUpdateMutex);
        if (onUpdate)
            onUpdate();
    }

private:
    std::function<void()> onUpdate;
    std::mutex onUpdateMutex;
};
}  // namespace GameEngine
