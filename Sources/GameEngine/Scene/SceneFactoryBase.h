#pragma once
#include <functional>
#include <memory>
#include <unordered_map>
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "Types.h"

namespace GameEngine
{
class Scene;
class InputManager;
class DisplayManager;

namespace Renderer
{
class RenderersManager;
}  // Renderer

typedef std::unique_ptr<Scene> ScenePtr;
typedef std::function<ScenePtr()> CreateFunction;
typedef std::unordered_map<std::string, CreateFunction> ScenesMap;
typedef std::unordered_map<uint32, std::string> OrderMap;
typedef std::unordered_map<std::string, uint32> IdMap;

class SceneFactoryBase
{
public:
    ScenePtr Create(const std::string&);
    ScenePtr Create(uint32);

    virtual ~SceneFactoryBase()
    {
    }

    uint32 ScenesSize()
    {
        return scenesMap_.size();
    }
    const std::string& GetSceneName(uint32 id);
    uint32 GetSceneId(const std::string& name);

    bool IsExist(uint32 name) const;
    bool IsExist(const std::string& name) const;

    void SetGraphicsApi(IGraphicsApi& graphicsApi);
    void SetInputManager(InputManager* input);
    void SetDisplayManager(DisplayManager* displayManager);
    void SetRenderersManager(Renderer::RenderersManager* manager);
    void SetPhysicsApi(Physics::IPhysicsApi& physicsApi);

protected:
    ScenePtr GetScene(const std::string& name);
    void SetMenagersAndApi(Scene* scene);
    void AddScene(const std::string&, CreateFunction);

private:
    IGraphicsApi* graphicsApi_;
    Physics::IPhysicsApi* physicsApi_;
    ScenesMap scenesMap_;
    OrderMap orderMap_;
    IdMap idMap_;
    InputManager* input_;
    DisplayManager* displayManager_;
    Renderer::RenderersManager* rendererMandager_;
};

typedef std::shared_ptr<SceneFactoryBase> SceneFactoryBasePtr;
}  // GameEngine
