#include <GameEngine/Scene/Scene.hpp>
#include <Logger/Log.h>

class WxEditorSceneDll : public GameEngine::Scene {
public:
 	WxEditorSceneDll()
        : GameEngine::Scene("WxEditorSceneDll")
    {
    }
    ~WxEditorSceneDll() override = default;

    int Initialize() override
    {
        camera.SetPosition(vec3(2, 2, 2));
        camera.LookAt(vec3(0, 0.5, 0));
        camera.UpdateMatrix();
        renderersManager_->GetDebugRenderer().Enable();

        /* LOG TO FIX*/  LOG_ERROR << ("Hello from WxEditorSceneDll");

        return 0;
    }
    void PostInitialize() override
    {
    }
    int Update(float) override
    {
        return 0;
    }
};

extern "C" GameEngine::Scene* CreateScene() {
    return new WxEditorSceneDll();
}