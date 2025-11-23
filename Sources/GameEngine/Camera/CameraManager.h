#pragma once
#include <Utils/IdPool.h>

#include <set>
#include <memory>
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
    using Cameras       = std::unordered_map<IdType, std::unique_ptr<ICamera>>;
    using CamerasView   = std::set<ICamera*>;
    using ActiveCameras = std::vector<std::pair<IdType, ICamera*>>;

    const CamerasView& GetCameras() const;
    ActiveCameras& GetActiveCameras();
    IdType AddCamera(std::unique_ptr<ICamera>);
    void RemoveCamera(IdType);
    void ActivateCamera(IdType);
    void ActivateCamera(ICamera*);
    void DeactivateCamera(IdType);
    void DeactivateCamera(ICamera*);
    std::optional<IdType> GetCameraId(ICamera*);
    void LockAll();
    void UnlockAll();

    void SetCameraAsMain(IdType);
    void SetCameraAsMain(ICamera*);

    ICamera* GetMainCamera();
    ICamera* GetCamera(IdType);
    ICamera* GetActiveCamera(IdType);

private:
    Utils::IdPool idPool;

    Cameras cameras;
    CamerasView camerasView;
    ActiveCameras activeCameras;
    ICamera* mainCamera;
};
}  // namespace GameEngine
