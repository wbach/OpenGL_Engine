#pragma once
#include <atomic>
#include <memory>
#include "GameEngine/Resources/ResourceManager.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "Thread.hpp"

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
    SceneLoader(GraphicsApi::IGraphicsApi&, IGpuResourceLoader&, DisplayManager&);
    ~SceneLoader();
    void Load(Scene&);

private:
    void Init();
    void LoadScene(Scene&);
    void IsLoading(bool);
    bool IsLoading();
    void UpdateScreen();
    void ScreenRenderLoop();
    void CheckObjectCount(Scene&);

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    DisplayManager& displayManager_;
    std::atomic_bool isLoading_;

    size_t objectCount_;
    size_t objectLoaded_;
    std::unique_ptr<LoadingScreenRenderer> loadingScreenRenderer;
    ResourceManager resorceManager_;
    IGpuResourceLoader& gpuLoader_;

    GeneralTexture* bgTexture_;
    GeneralTexture* circleTexture_;
};
}  // namespace GameEngine
