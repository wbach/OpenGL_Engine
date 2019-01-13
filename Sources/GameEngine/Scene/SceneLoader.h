#pragma once
#include <memory>
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "Mutex.hpp"
#include "Thread.hpp"

namespace GameEngine
{
class Scene;
class GpuObject;
class DisplayManager;
class ResourceManager;
class LoadingScreenRenderer;
class IShaderFactory;

class SceneLoader
{
public:
    SceneLoader(IGraphicsApi& graphicsApi, std::shared_ptr<DisplayManager>& displayManager,
                IShaderFactory& shaderFactory);
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
    IGraphicsApi& graphicsApi_;
    IShaderFactory& shaderFactory_;

    int objectCount;
    int objectLoaded;
    std::unique_ptr<LoadingScreenRenderer> loadingScreenRenderer;

    bool isLoading;
    std::mutex loadingMutex;

    std::shared_ptr<DisplayManager>& displayManager;
    ResourceManager resorceManager;
};
}  // namespace GameEngine
