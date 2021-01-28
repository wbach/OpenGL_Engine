#pragma once
#include <functional>
#include <memory>
#include <unordered_map>
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "Types.h"

namespace GameEngine
{
class Scene;
class EngineContext;

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

    virtual ~SceneFactoryBase() = default;

    inline uint32 ScenesSize();
    const std::string& GetSceneName(uint32 id);
    uint32 GetSceneId(const std::string& name);

    bool IsExist(uint32 name) const;
    bool IsExist(const std::string& name) const;
    void SetEngineContext(EngineContext&);

protected:
    ScenePtr GetScene(const std::string& name);
    void SetMenagersAndApi(Scene& scene);
    void AddScene(const std::string&, CreateFunction);

private:
    EngineContext* engineContext_;
    ScenesMap scenesMap_;
    OrderMap orderMap_;
    IdMap idMap_;
};

uint32 SceneFactoryBase::ScenesSize()
{
    return static_cast<uint32>(scenesMap_.size());
}
}  // namespace GameEngine
