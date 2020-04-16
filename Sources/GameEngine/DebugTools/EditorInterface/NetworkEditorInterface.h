#pragma once
#include <UtilsNetwork/Gateway.h>
#include <Common/Transform.h>
#include <functional>
#include <unordered_map>
#include <vector>

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
    virtual void AddObject(const std::string&) override;

private:
    typedef std::unordered_map<std::string, std::string> EntryParameters;
    void DefineCommands();
    void SetupCamera();
    void StartGatway();
    void PrepareDebugModels();
    void KeysSubscribtions();
    void KeysUnsubscribe();

    void NewUser(const std::string&, uint32);
    void DisconnectUser(uint32);
    void OnMessage(Network::UserId userId, std::unique_ptr<Network::IMessage>);
    void LoadSceneFromFile(const EntryParameters&);
    void GetCamera(const EntryParameters&);
    void GetObjectList(const EntryParameters&);
    void TransformReq(const EntryParameters&);
    void GetGameObjectComponentsListReq(const EntryParameters&);
    void SetGameObjectPosition(const EntryParameters&);
    void SetGameObjectRotation(const EntryParameters&);
    void SetGameObjectScale(const EntryParameters&);
    void CreateGameObject(const EntryParameters&);
    void GetComponentsList(const EntryParameters&);
    void AddComponent(const EntryParameters&);
    void GetComponentParams(const EntryParameters&);
    void StartScene(const EntryParameters&);
    void StopScene(const EntryParameters&);
    void ModifyComponentReq(const EntryParameters&);

    std::unordered_map<std::string, std::string> CreateParamMap(const std::vector<std::string>&);
    std::tuple<std::string, std::string> GetParamFromCommand(const std::string&);
    GameObject* GetGameObject(const std::string&);
    void UnsubscribeTransformUpdateIfExist();
    void SetFreeCamera();

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
};
}  // namespace GameEngine
