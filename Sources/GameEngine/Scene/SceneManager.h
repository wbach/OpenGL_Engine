#pragma once
#include <Mutex.hpp>
#include <queue>

#include "ISceneManager.h"
#include "SceneWrapper.h"

namespace GameEngine
{
class EngineContext;

class SceneManager : public ISceneManager
{
public:
    SceneManager(EngineContext&, std::unique_ptr<ISceneFactory>);
    ~SceneManager() override;

    void SetActiveScene(uint32) override;
    void SetActiveScene(const std::string&) override;
    Scene* GetActiveScene() override;

    void SetOnSceneLoadDone(OnSceneLoadDoneCallback) override;
    void Update() override;
    void Reset() override;
    const IdMap& GetAvaiableScenes() const override;
    void StopThread() override;

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
    void SetSceneToLoad(const std::string&);
    void SetSceneToLoad(uint32 id);
    void SetSceneContext(Scene* scene);

    template <class SceneNameOrId>
    void SetToWrapper(SceneNameOrId);

    void StartUpdateThreadIfNeeded();

private:
    EngineContext& engineContext_;
    std::unique_ptr<ISceneFactory> sceneFactory_;
    SceneWrapper sceneWrapper_;
    uint32 currentSceneId_;

    std::mutex eventMutex_;
    std::mutex processingEventMutex_;
    std::queue<GameEngine::SceneEvent> events_;
    std::queue<GameEngine::SceneEvent> processingEvents_;

    std::optional<IdType> updateSceneThreadId_;
    IdType fpsLimitParamSub_;
    bool isRunning_;
    OnSceneLoadDoneCallback onSceneLoadDoneCallback;
};
}  // namespace GameEngine
