#pragma once
#include <GameEngine/Scene/Scene.hpp>

namespace WxEditor
{
class NewScene : public GameEngine::Scene
{
public:
    NewScene();
    int Initialize() override;
    void PostInitialize() override;
    int Update(float) override;

private:
    void addPrimitive(GameEngine::PrimitiveType, const vec3& = vec3(0.f), const vec3& = vec3(1.f));
    void addDefaultLight();
    void addDefalutCamera();
};
}  // namespace WxEditor
