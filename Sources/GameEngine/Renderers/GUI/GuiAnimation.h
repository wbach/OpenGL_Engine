#pragma once
#include <Types.h>

#include "GuiElement.h"

namespace GameEngine
{
class GuiAnimation
{
public:
    enum class Status
    {
        inProgress,
        done
    };

    using Duration  = float;
    using DeltaTime = float;
    using Update    = std::function<void(GuiElement&, DeltaTime, Duration)>;
    GuiAnimation(std::unique_ptr<GuiElement> elemet, Duration duration, Update update)
        : updateFunc{update}
        , duration{duration}
        , elapsedTime{0.f}
        , element{std::move(elemet)}
    {
    }

    Status update(DeltaTime deltaTime)
    {
        elapsedTime += deltaTime;

        if (element and updateFunc and elapsedTime < duration)
        {
            updateFunc(*element, deltaTime, elapsedTime);
            return Status::inProgress;
        }

        return Status::done;
    }

    void reset()
    {
        elapsedTime = 0.f;
    }

private:
    Update updateFunc;
    Duration duration    = 1.f;
    Duration elapsedTime = 0.f;
    std::unique_ptr<GuiElement> element;
};
}  // namespace GameEngine
