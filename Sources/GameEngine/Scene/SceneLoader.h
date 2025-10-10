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
    SceneLoader(ISceneFactory&, GraphicsApi::IGraphicsApi&, IResourceManagerFactory&);
    ~SceneLoader();
    void Load(uint32);
    void Load(const std::string&);
    void UpdateLoadingScreen();
    bool IsReading() const;
    std::unique_ptr<Scene> GetResultScene();

private:
    void Init();
    void CleanUp();

    template <typename T>
    void LoadScene(const T&);
    bool IsGpuLoading() const;

private:
    ISceneFactory& sceneFactory_;
    std::unique_ptr<IResourceManager> resourceManager;
    GraphicsApi::IGraphicsApi& graphicsApi_;

    std::atomic_bool isReading;
    std::unique_ptr<LoadingScreenRenderer> loadingScreenRenderer;
    std::unique_ptr<Scene> resultScene;

    GeneralTexture* bgTexture_;
    GeneralTexture* circleTexture_;
};
}  // namespace GameEngine
