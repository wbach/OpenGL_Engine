#pragma once
#include "ISceneManager.h"
#include "SceneWrapper.h"

namespace GameEngine
{
class EngineContext;
class SceneLoader;

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

    void ProcessEvent(const ChangeSceneEvent&) override;
    void ProcessEvent(const ChangeSceneConfirmEvent&) override;

private:
    void UpdateScene(float dt);

    void LoadNextScene();
    void LoadPreviousScene();
    bool IsGpuLoading() const;

    template<typename T>
    void SetSceneToLoad(const T&);



    void SetSceneContext(Scene* scene);

    void StartUpdateThreadIfNeeded();

private:
    EngineContext& engineContext_;
    std::unique_ptr<ISceneFactory> sceneFactory_;
    SceneWrapper sceneWrapper_;
    uint32 currentSceneId_;

    std::optional<IdType> updateSceneThreadId_;
    IdType fpsLimitParamSub_;
    bool isRunning_;
    OnSceneLoadDoneCallback onSceneLoadDoneCallback;

    std::unique_ptr<SceneLoader> sceneLoader;
};
}  // namespace GameEngine
