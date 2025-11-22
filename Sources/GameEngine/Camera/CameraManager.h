#pragma once
#include <Utils/IdPool.h>

#include <memory>

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
    using ActiveCameras = std::vector<std::pair<IdType, ICamera*>>;

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
    ActiveCameras activeCameras;
    ICamera* mainCamera;
};
}  // namespace GameEngine
