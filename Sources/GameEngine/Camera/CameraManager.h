#pragma once
#include <Utils/IdPool.h>

#include <memory>
#include <set>
#include <unordered_map>

#include "Types.h"

struct Rotation;

namespace GameEngine
{
class ICamera;
class IProjection;

class CameraManager
{
public:
    using NotOwnedCameras = std::unordered_map<IdType, ICamera*>;
    using Cameras         = std::unordered_map<IdType, std::unique_ptr<ICamera>>;
    using CamerasView     = std::set<ICamera*>;
    using ActiveCameras   = std::vector<std::pair<IdType, ICamera*>>;

    const CamerasView& GetCameras() const;
    ActiveCameras& GetActiveCameras();
    IdType AddCamera(std::unique_ptr<ICamera>);
    IdType AddCamera(ICamera*);
    void RemoveCamera(ICamera*);
    void RemoveCamera(IdType);
    void ActivateCamera(IdType);
    void ActivateCamera(ICamera*);
    void DeactivateCamera(IdType);
    void DeactivateCamera(ICamera*);
    void DeactivateAllAdditionalCameras();
    std::optional<IdType> GetCameraId(ICamera*);
    void LockAll();
    void UnlockAll();

    void SetCameraAsMain(IdType);
    void SetCameraAsMain(ICamera*);

    ICamera* GetMainCamera();
    ICamera* GetCamera(IdType);
    ICamera* GetActiveCamera(IdType);
    bool IsCameraActive(ICamera*) const;

    void MainCameraCouldbeChange(bool);

private:
    Utils::IdPool idPool;

    Cameras cameras;
    NotOwnedCameras notOwnedCameras;
    CamerasView camerasView;
    ActiveCameras activeCameras;
    ICamera* mainCamera;

    bool mainCameraCouldBeChanged{true};
};
}  // namespace GameEngine
