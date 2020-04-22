#pragma once
#include <Common/Transform.h>
#include <Input/KeysSubscriptionsManager.h>
#include <Time/Timer.h>
#include <UtilsNetwork/Gateway.h>

#include <functional>
#include <unordered_map>
#include <vector>
#include "GameEngine/Camera/ICamera.h"
#include "IEditorInterface.hpp"

namespace GameEngine
{
class Scene;
class GameObject;
class DragObject;

class NetworkEditorInterface : public IEditorInterface
{
public:
    NetworkEditorInterface(Scene& scene);
    ~NetworkEditorInterface();
    virtual void Run() override;
    virtual void AddObject(const std::string&) override;

private:
    typedef std::unordered_map<std::string, std::string> EntryParameters;
    void DefineCommands();
    void SetupCamera();
    void StartGatway();
    void PrepareDebugModels();
    void KeysSubscribtions();
    void KeysUnsubscribe();
    void NotifIfObjectIsChanged();
    void NotifSelectedTransformIsChaned();
    void NotifSelectedCameraIsChaned();
    void SetGameObjectPosition(GameObject&, const vec3&);
    void SetGameObjectRotation(GameObject&, const vec3&);
    void IncreseGameObjectRotation(GameObject&, const vec3&);
    vec3 GetRotationValueBasedOnKeys(float, float);

    void NewUser(const std::string&, uint32);
    void DisconnectUser(uint32);
    void OnMessage(Network::UserId userId, std::unique_ptr<Network::IMessage>);
    void LoadSceneFromFile(const EntryParameters&);
    void SaveSceneToFile(const EntryParameters&);
    void GetCamera(const EntryParameters&);
    void GetObjectList(const EntryParameters&);
    void TransformReq(const EntryParameters&);
    void GetGameObjectComponentsListReq(const EntryParameters&);
    void SetGameObjectPosition(const EntryParameters&);
    void SetGameObjectRotation(const EntryParameters&);
    void SetGameObjectScale(const EntryParameters&);
    void CreateGameObject(const EntryParameters&);
    void DeleteGameObject(const EntryParameters&);
    void RenameGameObject(const EntryParameters&);
    void CreateGameObjectWithModel(const EntryParameters&);
    void LoadPrefab(const EntryParameters&);
    void GetComponentsList(const EntryParameters&);
    void AddComponent(const EntryParameters&);
    void GetComponentParams(const EntryParameters&);
    void SetPhysicsVisualization(const EntryParameters&);
    void SelectGameObject(const EntryParameters&);
    void StartScene();
    void StopScene();
    void ModifyComponentReq(const EntryParameters&);
    void GetRunningStatus(const EntryParameters&);
    void ReloadScene(const EntryParameters&);
    void ClearAll(const EntryParameters&);
    void ClearAllGameObjects(const EntryParameters&);

    std::unordered_map<std::string, std::string> CreateParamMap(const std::vector<std::string>&);
    std::tuple<std::string, std::string> GetParamFromCommand(const std::string&);
    GameObject* GetGameObject(const std::string&);
    void UnsubscribeTransformUpdateIfExist();
    void UnsubscribeCameraUpdateIfExist();
    void SetOrignalCamera();

private:
    Scene& scene_;
    Network::Gateway gateway_;
    std::unordered_map<std::string, std::function<void(const EntryParameters&)>> commands_;
    std::optional<uint32> transformChangeSubscriptionId_;
    std::optional<uint32> cameraChangeSubscriptionId_;
    common::Transform* transformChangeSubscription_;
    common::Transform arrowsIndicatorTransform_;
    uint32 keyDownSub_;
    uint32 keyUpSub_;
    uint32 scrollKeyUpSub_;
    uint32 scrollKeyDownSub_;
    uint32 cameraLockUnlockKeySubscribtion_;
    GameObject* selectedGameObject_;
    uint32 userId_;
    std::unique_ptr<DragObject> dragObject_;
    uint32 threadId_;

    Utils::Timer cameraTimer_;
    Utils::Timer transformTimer_;
    Input::KeysSubscriptionsManager keysSubscriptionsManager_;
    ICamera* sceneCamera_;
    bool running_;
};
}  // namespace GameEngine
