#pragma once
#include <functional>
#include <string>

#include "ISceneFactory.h"
#include "Types.h"

namespace GameEngine
{
struct ChangeSceneEvent;
struct ChangeSceneConfirmEvent;
class ISceneManager
{
public:
    using OnSceneLoadDoneCallback = std::function<void()>;

    virtual ~ISceneManager() = default;

    virtual Scene* GetActiveScene()                           = 0;
    virtual void SetOnSceneLoadDone(OnSceneLoadDoneCallback)  = 0;
    virtual void Update()                                     = 0;
    virtual void SetActiveScene(const std::string&)           = 0;
    virtual void SetActiveScene(uint32)                       = 0;
    virtual void Reset()                                      = 0;
    virtual void StopThread()                                 = 0;
    virtual const IdMap& GetAvaiableScenes() const            = 0;
    virtual void ProcessEvent(const ChangeSceneEvent&)        = 0;
    virtual void ProcessEvent(const ChangeSceneConfirmEvent&) = 0;
};
}  // namespace GameEngine
