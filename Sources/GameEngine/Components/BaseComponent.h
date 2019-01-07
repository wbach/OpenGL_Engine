#pragma once
#include "IComponent.h"
#include "ComponentController.h"
#include "GameEngine/Time/Time.h"
#include <memory>

namespace GameEngine
{
struct ICamera;
class GameObject;
class IResourceManager;

namespace Physics
{
struct IPhysicsApi;
}  // Physics

namespace Renderer
{
class RenderersManager;
}  // Renderer

namespace Components
{
class BaseComponent : public IComponent
{
public:
    BaseComponent(ComponentsType type);
    virtual ~BaseComponent();
    virtual void ReqisterFunctions(){}
    void SetTime(Time* time);
    void SetObjectPtr(GameObject* ptr);
    void SetResourceManager(IResourceManager* manager);
    void SetRendererManager(Renderer::RenderersManager* manager);
    void SetComponentController(ComponentController* componentController);
    void SetCamera(std::shared_ptr<ICamera>* camera);
    void SetPhysicsApi(Physics::IPhysicsApi* api);
    inline ComponentsType GetType();

protected:
    inline void RegisterFunction(FunctionType, std::function<void()> func);
    inline ICamera* GetCamera();

protected:
    Time* time_                                   = nullptr;
    GameObject* thisObject                        = nullptr;
    IResourceManager* resourceManager_            = nullptr;
    Physics::IPhysicsApi* physicsApi_             = nullptr;
    Renderer::RenderersManager* renderersManager_ = nullptr;

private:
    std::shared_ptr<ICamera>* camera_         = nullptr;
    ComponentController* componentController_ = nullptr;
    std::unordered_map<uint32, FunctionType> ids_;
    ComponentsType type_;
};

void BaseComponent::RegisterFunction(FunctionType type, std::function<void()> func)
{
    if (componentController_ == nullptr)
        return;

    ids_[componentController_->RegisterFunction(type, func)] = type;
}
inline ICamera* BaseComponent::GetCamera()
{
    return camera_ ? camera_->get() : nullptr;
}
ComponentsType BaseComponent::GetType()
{
    return type_;
}
}  // Components
}  // GameEngine
