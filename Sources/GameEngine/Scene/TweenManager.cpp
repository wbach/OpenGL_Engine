#include "TweenManager.h"

#include "Logger/Log.h"
#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
void TweenManager::Update(float dt)
{
    for (auto it = activeTweens.begin(); it != activeTweens.end();)
    {
        if (it->Update(dt))
        {
            it = activeTweens.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
void TweenManager::Add(GameObject& gameObject, const common::TransformContext& target, float duration, EaseType type,
                       std::function<void()> onComplete)
{
    LOG_DEBUG << "Add tween gameobject = " << gameObject.GetName() << ", target = " << target << ", duration= " << duration
              << ", type= " << magic_enum::enum_name(type);

    activeTweens.push_back(Tween{.gameObject = gameObject,
                                 .source     = gameObject.GetWorldTransform().GetCurrentContext(),
                                 .target     = target,
                                 .easeType   = type,
                                 .onComplete = onComplete,
                                 .duration   = duration});
}
void TweenManager::Remove(const GameObject& obj)
{
    activeTweens.erase(std::remove_if(activeTweens.begin(), activeTweens.end(),
                                      [&obj](const Tween& t) { return t.gameObject.get().GetId() == obj.GetId(); }),
                       activeTweens.end());
}
}  // namespace GameEngine
