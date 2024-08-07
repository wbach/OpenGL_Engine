﻿#pragma once
#include <Common/Transform.h>
#include <Input/KeysSubscriptionsManager.h>
#include <Time/Timer.h>
#include <Utils/ThreadSync.h>
#include <UtilsNetwork/Gateway.h>

#include <functional>
#include <unordered_map>
#include <vector>

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/DebugTools/Painter/Painter.h"
#include "GameEngine/Renderers/DebugElements/DebugRenderer.h"
#include "IEditorInterface.hpp"

namespace GameEngine
{
class Scene;
class GameObject;
class DragObject;

class NetworkEditorInterface : public IEditorInterface
{
public:
    NetworkEditorInterface(Scene&, Utils::Thread::ThreadSync&);
    ~NetworkEditorInterface();
    virtual void Run() override;
    virtual void AddObject(const std::string&) override;

private:
    void MainLoop();
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
    void IncreseGameObjectScale(GameObject&, const vec3&);
    vec3 GetRotationValueBasedOnKeys(float, float);
    vec3 GetPositionChangeValueBasedOnKeys(float, float);
    vec3 GetScaleChangeValueBasedOnKeys(float, float);

    void QuickSave();
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
    void SetPhysicsVisualizationAllObjcts(const EntryParameters&);
    void SetNormalsVisualization(const EntryParameters&);
    void SetLineRenderMode(const EntryParameters&);
    void SelectGameObject(const EntryParameters&);
    void GoCameraToObject(const EntryParameters&);
    void StartScene(const EntryParameters&);
    void StopScene(const EntryParameters&);
    void ModifyComponentReq(const EntryParameters&);
    void GetRunningStatus(const EntryParameters&);
    void ReloadScene(const EntryParameters&);
    void ClearAll(const EntryParameters&);
    void ClearAllGameObjects(const EntryParameters&);
    void EnableTerrainHeightPainter(const EntryParameters&);
    void EnableTerrainTexturePainter(const EntryParameters&);
    void EnablePlantPainter(const EntryParameters&);
    void DisableTerrainPainter(const EntryParameters&);
    void UpdateTerrainPainterParam(const EntryParameters&);
    void RecalculateTerrainNormals(const EntryParameters&);
    void ClearTerrainsBlendMap(const EntryParameters&);
    void GenerateTerrains(const EntryParameters&);
    void ControlTextureUsage(const EntryParameters&);
    void CreateTerrain(const EntryParameters&);
    void GenerateTerrainBlendMapToFile();
    void ReloadShaders(const EntryParameters&);
    void Takesnapshot(const EntryParameters&);
    void Exit(const EntryParameters&);
    void MoveObjectToCameraPosition(const EntryParameters&);
    void ModelPreviewRequest(const EntryParameters&);
    void ChangeGameObjectParent(const EntryParameters&);
    void CloneGameObject(const EntryParameters&);
    void CloneGameObjectInstancesWithRandomPosition(const EntryParameters&);
    void CreatePrefabFromObject(const EntryParameters&);
    void CreateTerrainTranstion(const EntryParameters&);

    void StartScene();
    void StopScene();

    EntryParameters CreateParamMap(const std::vector<std::string>&);
    std::tuple<std::string, std::string> GetParamFromCommand(const std::string&);
    GameObject* GetGameObject(const std::string&);
    void UnsubscribeTransformUpdateIfExist();
    void UnsubscribeCameraUpdateIfExist();
    void SetOrignalCamera();
    std::optional<uint32> AddGameObject(const EntryParameters&, std::unique_ptr<GameObject>&);
    Painter::EntryParamters GetPainterEntryParameters();
    void SetDeubgRendererState(DebugRenderer::RenderState, const EntryParameters&);
    void ObjectControlAction(float direction, float rotationSpeed = 2.f, float moveSpeed = 0.2f,
                             float scaleSpeed = 0.1f);
    void CreateDragObject(GameObject&);
    void ReleaseDragObject();
    void SetSelectedGameObject(GameObject*);
    void UseSelectedGameObject(std::function<void(GameObject&)>, std::function<void()> = nullptr);
    void UpdateDragObject();
    void PaintTerrain();
    void UpdateArrowsIndicatorPosition();
    void SendObjectCreatedNotf(const GameObject&);
    void DeleteGameObject(IdType);
    void DeleteGameObject(GameObject&);

private:
    void setParamIfExitst(const EntryParameters&, const std::string&, bool&);
    void setParamIfExitst(const EntryParameters&, const std::string&, int&);
    void setParamIfExitst(const EntryParameters&, const std::string&, uint32&);
    void setParamIfExitst(const EntryParameters&, const std::string&, float&);
    void setParamIfExitst(const EntryParameters&, const std::string&, double&);

    template <typename T>
    void setParamIfExitst(const EntryParameters&, const std::string&, std::optional<T>&);

private:
    Scene& scene_;
    Utils::Thread::ThreadSync& threadSync_;
    Network::Gateway gateway_;
    std::unordered_map<std::string, std::function<void(const EntryParameters&)>> commands_;
    std::optional<uint32> transformChangeSubscriptionId_;
    std::optional<uint32> cameraChangeSubscriptionId_;
    common::Transform* transformChangeSubscription_;

    GameObject* selectedGameObject_;
    uint32 userId_;
    uint32 threadId_;

    Utils::Timer cameraTimer_;
    Utils::Timer transformTimer_;
    Input::KeysSubscriptionsManager keysSubscriptionsManager_;
    bool running_;

    std::mutex selectedGameObjectMutex_;
    std::mutex dragObjectMutex_;
    std::unique_ptr<DragObject> dragObject_;
    std::mutex terrainPainterMutex_;
    std::unique_ptr<Painter> terrainPainter_;
    Utils::Timer terrainPainterTimer_;

    Model* arrowsIndicator_;
    Model* brushCircle_;
    int32 brushSize_{1};

    common::Transform arrowsIndicatorTransform_;
    common::Transform brushCircleTransform_;

    std::optional<IdType> cameraEditorId_;
};
}  // namespace GameEngine
