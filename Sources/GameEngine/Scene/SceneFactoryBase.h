#pragma once
#include "ISceneFactory.h"

namespace GameEngine
{
class SceneFactoryBase : public ISceneFactory
{
public:
    void ReadScenes();
    ScenePtr Create(const std::string&) override;
    ScenePtr Create(uint32) override;
    ScenePtr CreateSceneBasedOnFile(const File&) override;

    inline uint32 ScenesSize() override;
    const std::string& GetSceneName(uint32 id) override;
    uint32 GetSceneId(const std::string& name) override;
    const IdMap& GetAvaiableScenes() const override;

    bool IsExist(uint32 name) const override;
    bool IsExist(const std::string& name) const override;

    void SetEngineContext(EngineContext&) override;
    void Clear() override;

protected:
    ScenePtr GetScene(const std::string& name);
    void SetMenagersAndApi(Scene& scene);

    template <typename Method>
    void AddScene(const std::string& sceneName, const Method& method)
    {
        auto currentId = static_cast<uint32>(scenesMap_.size());

        idMap_[sceneName]     = currentId;
        orderMap_[currentId]  = sceneName;
        scenesMap_[sceneName] = method;
    }

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
