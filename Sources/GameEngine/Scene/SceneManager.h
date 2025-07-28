#pragma once
#include <queue>

#include <Mutex.hpp>
#include "GameEngine/Engine/EngineEvent.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "SceneContext.h"
#include "SceneEvents.h"
#include "SceneWrapper.h"
#include "ISceneManager.h"

namespace GameEngine
{
class EngineContext;

class SceneManager : public ISceneManager
{
public:
    SceneManager(EngineContext&, std::unique_ptr<ISceneFactory>);
    ~SceneManager();

    Scene* GetActiveScene();
    void Update();
    void SetActiveScene(const std::string& name);
    void Reset();
    void Stop();
    const IdMap& GetAvaiableScenes() const;

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
};
}  // namespace GameEngine
