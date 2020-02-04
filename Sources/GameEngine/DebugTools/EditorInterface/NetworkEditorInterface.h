#pragma once
#include <UtilsNetwork/Gateway.h>
#include "IEditorInterface.hpp"
#include <unordered_map>
#include <vector>
#include <functional>

namespace GameEngine
{
class Scene;
class GameObject;

class NetworkEditorInterface : public IEditorInterface
{
public:
    NetworkEditorInterface(Scene& scene);
    ~NetworkEditorInterface();
    virtual void AddObject(const std::string&) override;

private:
    void NewUser(const std::string&, uint32);
    void DisconnectUser(uint32);
    void OnMessage(Network::UserId userId, std::unique_ptr<Network::IMessage>);
    void LoadSceneFromFile(const std::vector<std::string>&);
    void GetObjectList(const std::vector<std::string>&);
    void TransformReq(const std::vector<std::string>&);
    void GetGameObjectComponentsListReq(const std::vector<std::string>&);

    std::tuple<std::string, std::string> GetParamFromCommand(const std::string&);
    GameObject* GetGameObjectBasedOnParam(const std::string&);

private:
    Scene& scene_;
    Network::Gateway gateway_;
    std::thread networkThread_;
    std::atomic_bool isRunning_;
    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> commands_;

    uint32 userId_;
};
}  // namespace GameEngine
