#pragma once
#include <Types.h>

#include <atomic>
#include <memory>

namespace GraphicsApi
{
class IGraphicsApi;
}
namespace GameEngine
{
class Scene;
class ISceneFactory;
class GpuObject;
class DisplayManager;
class IResourceManager;
class LoadingScreenRenderer;
class IResourceManagerFactory;
class IGpuResourceLoader;
class GeneralTexture;

class SceneLoader
{
public:
    SceneLoader(ISceneFactory&, GraphicsApi::IGraphicsApi&, IGpuResourceLoader&, DisplayManager&, IResourceManagerFactory&);
    ~SceneLoader();
    std::unique_ptr<Scene> Load(uint32);
    std::unique_ptr<Scene> Load(const std::string&);

private:
    void Init();
    void CleanUp();

    template <typename T>
    std::unique_ptr<Scene> LoadScene(T);

    bool IsGpuLoading();
    void UpdateScreen();
    void ScreenRenderLoop();
    void CheckObjectCount(Scene&);

private:
    ISceneFactory& sceneFactory_;
    GraphicsApi::IGraphicsApi& graphicsApi_;
    DisplayManager& displayManager_;
    std::atomic_bool isReading;

    size_t objectCount_;
    std::unique_ptr<LoadingScreenRenderer> loadingScreenRenderer;
    std::unique_ptr<IResourceManager> resourceManager;
    IGpuResourceLoader& gpuLoader_;

    GeneralTexture* bgTexture_;
    GeneralTexture* circleTexture_;
};
}  // namespace GameEngine
