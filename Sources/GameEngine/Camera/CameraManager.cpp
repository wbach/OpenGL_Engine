#include "CameraManager.h"

#include <Logger/Log.h>

#include "GameEngine/Camera/ICamera.h"

namespace GameEngine
{
CameraManager::ActiveCameras& CameraManager::GetActiveCameras()
{
    return activeCameras;
}
IdType CameraManager::AddCamera(std::unique_ptr<ICamera> camera)
{
    auto id = idPool.getId();
    camerasView.insert(camera.get());
    cameras.insert({id, std::move(camera)});
    return id;
}
void CameraManager::RemoveCamera(IdType id)
{
    DeactivateCamera(id);
    auto iter = cameras.find(id);
    if (iter != cameras.end())
    {
        camerasView.erase(iter->second.get());
        cameras.erase(iter);
    }
}
void CameraManager::ActivateCamera(IdType id)
{
    auto iter = cameras.find(id);
    if (iter != cameras.end())
    {
        activeCameras.push_back({id, iter->second.get()});
    }
}
void CameraManager::ActivateCamera(ICamera* ptr)
{
    for (auto& [id, camera] : cameras)
    {
        if (camera.get() == ptr)
        {
            activeCameras.push_back({id, ptr});
        }
    }
}
void CameraManager::DeactivateCamera(IdType id)
{
    auto iter = std::find_if(activeCameras.begin(), activeCameras.end(), [id](auto pair) { return pair.first == id; });

    if (iter != activeCameras.end())
    {
        if (mainCamera == iter->second)
        {
            mainCamera = nullptr;
        }
        activeCameras.erase(iter);
    }
}

void CameraManager::DeactivateCamera(ICamera* camera)
{
    auto iter = std::find_if(activeCameras.begin(), activeCameras.end(), [camera](auto pair) { return pair.second == camera; });

    if (iter != activeCameras.end())
    {
        activeCameras.erase(iter);
    }
}

void CameraManager::SetCameraAsMain(IdType id)
{
    if (auto camera = GetActiveCamera(id))
    {
        mainCamera = camera;
    }
    else
    {
        LOG_WARN << "Try set no active camera as main";
    }
}
ICamera* CameraManager::GetMainCamera()
{
    if (not mainCamera)
    {
        LOG_ERROR << "Main camera not set";
    }
    return mainCamera;
}
ICamera* CameraManager::GetCamera(IdType id)
{
    auto iter = cameras.find(id);
    if (iter != cameras.end())
    {
        return iter->second.get();
    }

    return nullptr;
}
ICamera* CameraManager::GetActiveCamera(IdType id)
{
    auto iter = std::find_if(activeCameras.begin(), activeCameras.end(), [id](auto pair) { return pair.first == id; });

    if (iter != activeCameras.end())
    {
        return iter->second;
    }

    return nullptr;
}
void CameraManager::LockAll()
{
    for (auto [_, cameraPtr] : activeCameras)
    {
        cameraPtr->Lock();
    }
}
void CameraManager::UnlockAll()
{
    for (auto [_, cameraPtr] : activeCameras)
    {
        cameraPtr->Unlock();
    }
}

std::optional<IdType> CameraManager::GetCameraId(ICamera* camera)
{
    auto iter = std::find_if(activeCameras.begin(), activeCameras.end(), [camera](auto pair) { return pair.second == camera; });

    if (iter != activeCameras.end())
    {
        return iter->first;
    }

    return {};
}
void CameraManager::SetCameraAsMain(ICamera* camera)
{
    auto iter = std::find_if(activeCameras.begin(), activeCameras.end(), [camera](auto pair) { return pair.second == camera; });

    if (iter != activeCameras.end())
    {
        mainCamera = camera;
    }
}
const CameraManager::CamerasView& CameraManager::GetCameras() const
{
    return camerasView;
}
void CameraManager::RemoveCamera(ICamera* ptr)
{
    if (auto id = GetCameraId(ptr))
    {
        RemoveCamera(*id);
    }
}
}  // namespace GameEngine