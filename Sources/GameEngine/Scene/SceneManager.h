#pragma once
#include <queue>

#include <Mutex.hpp>
#include "GameEngine/Engine/EngineEvent.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "SceneContext.h"
#include "SceneEvents.h"
#include "SceneFactoryBase.h"
#include "SceneWrapper.h"

namespace GameEngine
{
class EngineContext;

class SceneManager
{
public:
    SceneManager(EngineContext&, std::unique_ptr<SceneFactoryBase>);
    ~SceneManager();

    Scene* GetActiveScene();
    void InitActiveScene();
    void Update();
    void SetActiveScene(const std::string& name);
    void Reset();
    void SetFactor();
    void Stop();

private:
    void TakeEvents();
    void ProccessEvents();
    void UpdateScene(float dt);
    void AddSceneEvent(const SceneEvent&);
    void AddEventToProcess(const SceneEvent&);
    std::optional<GameEngine::SceneEvent> GetSceneEvent();
    std::optional<GameEngine::SceneEvent> GetProcessingEvent();

    void LoadNextScene();
    void LoadPreviousScene();
    void LoadScene(const std::string&);
    void LoadScene(uint32 id);
    void SetSceneContext(Scene* scene);

    template <class T>
    void JustLoadScene(T scene);

    void Start();

private:
    EngineContext& engineContext_;
    std::unique_ptr<SceneFactoryBase> sceneFactory_;
    SceneWrapper sceneWrapper_;
    uint32 currentSceneId_;

    std::mutex eventMutex_;
    std::mutex processingEventMutex_;
    std::queue<GameEngine::SceneEvent> events_;
    std::queue<GameEngine::SceneEvent> processingEvents_;

    uint32 updateSceneThreadId_;
    bool isRunning_;
};
}  // namespace GameEngine
