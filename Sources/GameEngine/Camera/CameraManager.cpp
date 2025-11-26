#include "CameraManager.h"

#include <Logger/Log.h>

#include <algorithm>

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
IdType CameraManager::AddCamera(ICamera* camera)
{
    auto id = idPool.getId();
    camerasView.insert(camera);
    notOwnedCameras.insert({id, std::move(camera)});
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
        return;
    }

    auto notOwnedCamerasIter = notOwnedCameras.find(id);
    if (notOwnedCamerasIter != notOwnedCameras.end())
    {
        camerasView.erase(notOwnedCamerasIter->second);
        notOwnedCameras.erase(notOwnedCamerasIter);
    }
}
void CameraManager::RemoveCamera(ICamera* ptr)
{
    DeactivateCamera(ptr);
    camerasView.erase(ptr);

    auto iter = std::find_if(cameras.begin(), cameras.end(), [ptr](const auto& pair) { return pair.second.get() == ptr; });
    if (iter != cameras.end())
    {
        cameras.erase(iter);
        return;
    }
    auto notOwnedIter =
        std::find_if(notOwnedCameras.begin(), notOwnedCameras.end(), [ptr](const auto& pair) { return pair.second == ptr; });
    if (notOwnedIter != notOwnedCameras.end())
    {
        notOwnedCameras.erase(notOwnedIter);
        return;
    }
}
void CameraManager::ActivateCamera(IdType id)
{
    auto iter = cameras.find(id);
    if (iter != cameras.end())
    {
        activeCameras.push_back({id, iter->second.get()});
        LOG_DEBUG << "Camera activated";
        return;
    }

    auto notOwnedIter = notOwnedCameras.find(id);
    if (notOwnedIter != notOwnedCameras.end())
    {
        activeCameras.push_back({id, notOwnedIter->second});
        LOG_DEBUG << "Camera activated";
    }
}
void CameraManager::ActivateCamera(ICamera* ptr)
{
    for (auto& [id, camera] : cameras)
    {
        if (camera.get() == ptr)
        {
            activeCameras.push_back({id, ptr});
            LOG_DEBUG << "Camera activated";
            return;
        }
    }

    for (auto& [id, camera] : notOwnedCameras)
    {
        if (camera == ptr)
        {
            activeCameras.push_back({id, ptr});
            LOG_DEBUG << "Camera activated";
            return;
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
        LOG_DEBUG << "Camera deactivated";
    }
}

void CameraManager::DeactivateCamera(ICamera* camera)
{
    auto iter = std::find_if(activeCameras.begin(), activeCameras.end(), [camera](auto pair) { return pair.second == camera; });

    if (iter != activeCameras.end())
    {
        if (mainCamera == camera)
        {
            mainCamera = nullptr;
        }
        activeCameras.erase(iter);
        LOG_DEBUG << "Camera deactivated";
    }
}

void CameraManager::SetCameraAsMain(IdType id)
{
    if (not mainCameraCouldBeChanged)
    {
        LOG_DEBUG << "Main camera could not be changed. Is probably locked by other camera.";
        return;
    }
    if (auto camera = GetActiveCamera(id))
    {
        mainCamera = camera;
    }
    else
    {
        LOG_WARN << "Try set no active camera as main";
    }
}
void CameraManager::SetCameraAsMain(ICamera* camera)
{
    if (not mainCameraCouldBeChanged)
    {
        LOG_DEBUG << "Main camera could not be changed. Is probably locked by other camera.";
        return;
    }
    auto iter = std::find_if(activeCameras.begin(), activeCameras.end(), [camera](auto pair) { return pair.second == camera; });

    if (iter != activeCameras.end())
    {
        mainCamera = camera;
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

    auto niter = notOwnedCameras.find(id);
    if (niter != notOwnedCameras.end())
    {
        return niter->second;
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
    for (auto& [_, camera] : cameras)
    {
        camera->Lock();
    }

    for (auto& [_, camera] : notOwnedCameras)
    {
        camera->Lock();
    }
}
void CameraManager::UnlockAll()
{
    for (auto& [_, camera] : cameras)
    {
        camera->Unlock();
    }

    for (auto& [_, camera] : notOwnedCameras)
    {
        camera->Unlock();
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
const CameraManager::CamerasView& CameraManager::GetCameras() const
{
    return camerasView;
}
bool CameraManager::IsCameraActive(ICamera* camera) const
{
    return activeCameras.end() !=
           std::find_if(activeCameras.begin(), activeCameras.end(), [camera](auto pair) { return pair.second == camera; });
}
void CameraManager::MainCameraCouldbeChange(bool v)
{
    mainCameraCouldBeChanged = v;
}
void CameraManager::DeactivateAllAdditionalCameras()
{
    for (auto iter = activeCameras.begin(); iter != activeCameras.end();)
    {
        if (iter->second == mainCamera)
        {
            ++iter;
            continue;
        }

        iter = activeCameras.erase(iter);
    }
}
}  // namespace GameEngine