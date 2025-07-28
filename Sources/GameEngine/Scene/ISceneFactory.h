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
typedef std::variant<CreateFunction, File> CreationMethod;
typedef std::unordered_map<std::string, CreationMethod> ScenesMap;
typedef std::unordered_map<uint32, std::string> OrderMap;
typedef std::unordered_map<std::string, uint32> IdMap;

class ISceneFactory
{
public:
    virtual ~ISceneFactory() = default;

    virtual ScenePtr Create(const std::string&) = 0;
    virtual ScenePtr Create(uint32) = 0;
    virtual ScenePtr CreateSceneBasedOnFile(const File&) = 0;

    virtual inline uint32 ScenesSize() = 0;
    virtual const std::string& GetSceneName(uint32 id) = 0;
    virtual uint32 GetSceneId(const std::string& name) = 0;
    virtual const IdMap& GetAvaiableScenes() const = 0;

    virtual bool IsExist(uint32 name) const = 0;
    virtual bool IsExist(const std::string& name) const = 0;

    virtual void SetEngineContext(EngineContext&) = 0;
    virtual void Clear() = 0;
};
} // namespace GameEngine
