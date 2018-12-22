#include "SceneLoader.h"
#include "../Display/DisplayManager.hpp"
#include "../Renderers/LodingScreenRenderer.h"
#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "Logger/Log.h"
#include "Scene.hpp"

namespace GameEngine
{
SceneLoader::SceneLoader(IGraphicsApiPtr graphicsApi, std::shared_ptr<DisplayManager>& displayManager, IShaderFactory& shaderFactory)
    : graphicsApi_(graphicsApi)
    , shaderFactory_(shaderFactory)
    , objectCount(0)
    , objectLoaded(0)
    , displayManager(displayManager)
    , resorceManager(graphicsApi)
{
}

SceneLoader::~SceneLoader()
{
}

bool SceneLoader::Load(Scene* scene)
{
    if (displayManager == nullptr)
        return false;

    Init();

    isLoading = true;
    std::thread loading_thread(&SceneLoader::LoadScene, this, scene);
    OpenGLLoadingPass(scene, loading_thread);
    return true;
}

void SceneLoader::OpenGLLoadingPass(Scene* scene, std::thread& loading_thread)
{
    CheckObjectCount(scene);
    LoadingLoop(scene);
    loading_thread.join();
    PostLoadingPass(scene);
}

void SceneLoader::Init()
{
    auto circleTexture =
        resorceManager.GetTextureLaoder().LoadTextureImmediately("GUI/circle2.png", false, ObjectTextureType::MATERIAL);
    auto bgtexture = resorceManager.GetTextureLaoder().LoadTextureImmediately(
        "GUI/black-knight-dark-souls.png", false, ObjectTextureType::MATERIAL, TextureFlip::Type::VERTICAL);
    loadingScreenRenderer =
        std::make_unique<LoadingScreenRenderer>(graphicsApi_, bgtexture, circleTexture, shaderFactory_);
    loadingScreenRenderer->Init();
}

void SceneLoader::SetIsLoading(bool is)
{
    std::lock_guard<std::mutex> lock(loadingMutex);
    isLoading = is;
}

bool SceneLoader::GetIsLoading()
{
    std::lock_guard<std::mutex> lock(loadingMutex);
    return isLoading;
}

bool SceneLoader::ProccesLoadingLoop(GpuObject *obj)
{
    if (displayManager != nullptr)
        displayManager->ProcessEvents();

    auto load = GetIsLoading();
    if (LoadObject(obj))
        load = true;
    UpdateScreen();

    return load;
}

bool SceneLoader::LoadObject(GpuObject *obj)
{
    if (obj == nullptr)
        return false;

    obj->GpuLoadingPass();
    ++objectLoaded;

    // std::cout << "Loading... " + std::to_string((int)((float)objectLoaded / (float)objectCount) *100.f) + "%" <<
    // std::endl;
    return true;
}

void SceneLoader::UpdateScreen()
{
    if (loadingScreenRenderer != nullptr)
        loadingScreenRenderer->Render(nullptr);

    if (displayManager == nullptr)
        return;

    displayManager->Update();
}

void SceneLoader::LoadingLoop(Scene* scene)
{
    bool load       = true;
    auto& objLoader = scene->GetResourceManager().GetGpuResourceLoader();

    while (load)
        load = ProccesLoadingLoop(objLoader.GetObjectToGpuLoadingPass());
}

void SceneLoader::CheckObjectCount(Scene* scene)
{
    objectCount = scene->objectCount;
    if (objectCount <= 0)
        objectCount = 1;
}

void SceneLoader::PostLoadingPass(Scene* scene)
{
    bool load       = true;
    auto& objLoader = scene->GetResourceManager().GetGpuResourceLoader();

    while (load)
        load = ProccesLoadingLoop(objLoader.GetObjectToGpuPostLoadingPass());
}

void SceneLoader::LoadScene(Scene* scene)
{
    if (scene == nullptr)
        return;

    auto start = std::chrono::high_resolution_clock::now();
    scene->Init();
    auto end = std::chrono::high_resolution_clock::now();
    Log("Scene loading time: " +
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.f) + "s.");
    SetIsLoading(false);
}
}  // GameEnigne
