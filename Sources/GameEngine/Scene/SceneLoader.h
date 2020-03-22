#pragma once
#include <memory>
#include "GraphicsApi/IGraphicsApi.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "Thread.hpp"
#include <atomic>

namespace GameEngine
{
class Scene;
class GpuObject;
class DisplayManager;
class ResourceManager;
class LoadingScreenRenderer;

class SceneLoader
{
public:
    SceneLoader(GraphicsApi::IGraphicsApi& graphicsApi, DisplayManager& displayManager);
    ~SceneLoader();
    bool Load(Scene* scene);

private:
    void Init();
    void LoadScene(Scene* scene);
    void SetIsLoading(bool is);
    bool GetIsLoading();
    bool ProccesLoadingLoop(GpuObject* obj);
    bool LoadObject(GpuObject* obj);
    void UpdateScreen();
    void LoadingLoop(Scene* scene);
    void CheckObjectCount(Scene* scene);
    void PostLoadingPass(Scene* scene);
    void OpenGLLoadingPass(Scene* scene, std::thread& loading_thread);

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;

    int objectCount;
    int objectLoaded;
    std::unique_ptr<LoadingScreenRenderer> loadingScreenRenderer;

    std::atomic_bool isLoading;

    DisplayManager& displayManager;
    ResourceManager resorceManager;
};
}  // namespace GameEngine
