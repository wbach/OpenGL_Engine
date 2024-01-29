#include "CameraWrapper.h"

#include <Utils/Mutex.hpp>

namespace GameEngine
{
namespace
{
std::mutex cameraMutex;
}
CameraWrapper::CameraWrapper()
    : setCameras_({&baseCamera_})
{
}

CameraWrapper::CameraWrapper(ICamera &camera)
    : setCameras_({&baseCamera_, &camera})
{
}

IdType CameraWrapper::add(std::unique_ptr<ICamera> camera)
{
    std::lock_guard<std::mutex> m(cameraMutex);
    auto id = camerasIdPool_.getId();
    avaiableCameras_.insert({id, std::move(camera)});
    return id;
}

IdType CameraWrapper::addAndSet(std::unique_ptr<ICamera> camera)
{
    std::lock_guard<std::mutex> m(cameraMutex);
    auto cameraPtr = camera.get();

    auto id = camerasIdPool_.getId();
    avaiableCameras_.insert({id, std::move(camera)});
    setCamera(cameraPtr);
    return id;
}

void CameraWrapper::remove(IdType id)
{
    std::lock_guard<std::mutex> m(cameraMutex);
    auto avaiableCameraIter = avaiableCameras_.find(id);
    if (avaiableCameraIter != avaiableCameras_.end())
    {
        auto avaiableCameraPtr = avaiableCameraIter->second.get();
        removeCamera(avaiableCameraPtr);
        avaiableCameras_.erase(id);
        camerasIdPool_.releaseId(id);
    }
}

void CameraWrapper::set(IdType id)
{
    std::lock_guard<std::mutex> m(cameraMutex);
    auto avaiableCameraIter = avaiableCameras_.find(id);
    if (avaiableCameraIter != avaiableCameras_.end())
    {
        auto avaiableCameraPtr = avaiableCameraIter->second.get();
        auto setCameraiter     = std::find(setCameras_.begin(), setCameras_.end(), avaiableCameraPtr);
        if (setCameraiter != setCameras_.end())
        {
            setCameras_.erase(setCameraiter);
        }
        setCamera(avaiableCameraPtr);
    }
}

void CameraWrapper::pop()
{
    if (setCameras_.empty() or setCameras_.back() == &baseCamera_)
        return;

    setCameras_.pop_back();
}

ICamera *CameraWrapper::Get() const
{
    return setCameras_.back();
}

void CameraWrapper::setCamera(ICamera *cameraPtr)
{
    if (not setCameras_.empty())
    {
        moveSubscribtionsToCurrentCamera(*setCameras_.back(), *cameraPtr);
    }

    setCameras_.push_back(cameraPtr);
    notifySubscribersWhenCameraChange();
}

void CameraWrapper::removeCamera(ICamera *camera)
{
    if (setCameras_.size() >= 2 and setCameras_.back() == camera)
    {
        moveSubscribtionsToCurrentCamera(*setCameras_.back(), *setCameras_[setCameras_.size() - 2]);
    }

    auto setCameraiter = std::find(setCameras_.begin(), setCameras_.end(), camera);
    if (setCameraiter != setCameras_.end())
    {
        setCameras_.erase(setCameraiter);
    }
}

void CameraWrapper::Update()
{
    std::lock_guard<std::mutex> m(cameraMutex);
    Get()->Update();
    Get()->UpdateMatrix();
}

bool CameraWrapper::IsLocked() const
{
    return Get()->IsLocked();
}

void CameraWrapper::Lock()
{
    std::lock_guard<std::mutex> m(cameraMutex);
    Get()->Lock();
}

void CameraWrapper::Unlock()
{
    std::lock_guard<std::mutex> m(cameraMutex);
    Get()->Unlock();
}

void CameraWrapper::UpdateMatrix()
{
    std::lock_guard<std::mutex> m(cameraMutex);
    Get()->UpdateMatrix();
}

float CameraWrapper::GetPitch() const
{
    std::lock_guard<std::mutex> m(cameraMutex);
    return Get()->GetPitch();
}

float CameraWrapper::GetYaw() const
{
    std::lock_guard<std::mutex> m(cameraMutex);
    return Get()->GetYaw();
}

const Rotation &CameraWrapper::GetRotation() const
{
    std::lock_guard<std::mutex> m(cameraMutex);
    return Get()->GetRotation();
}

const vec3 &CameraWrapper::GetPosition() const
{
    std::lock_guard<std::mutex> m(cameraMutex);
    return Get()->GetPosition();
}

const vec3 &CameraWrapper::GetDirection() const
{
    std::lock_guard<std::mutex> m(cameraMutex);
    return Get()->GetDirection();
}

const mat4 &CameraWrapper::GetTranslationMatrix() const
{
    std::lock_guard<std::mutex> m(cameraMutex);
    return Get()->GetTranslationMatrix();
}

const mat4 &CameraWrapper::GetRotationMatrix() const
{
    std::lock_guard<std::mutex> m(cameraMutex);
    return Get()->GetRotationMatrix();
}

const mat4 &CameraWrapper::GetViewMatrix() const
{
    std::lock_guard<std::mutex> m(cameraMutex);
    return Get()->GetViewMatrix();
}

void CameraWrapper::LookAt(const vec3 &at)
{
    std::lock_guard<std::mutex> m(cameraMutex);
    Get()->LookAt(at);
}

void CameraWrapper::SetYaw(float yaw)
{
    std::lock_guard<std::mutex> m(cameraMutex);
    Get()->SetYaw(yaw);
}

void CameraWrapper::SetPitch(float pitch)
{
    std::lock_guard<std::mutex> m(cameraMutex);
    Get()->SetPitch(pitch);
}

void CameraWrapper::SetRotation(const Rotation &rotation)
{
    std::lock_guard<std::mutex> m(cameraMutex);
    Get()->SetRotation(rotation);
}

void CameraWrapper::SetPosition(const vec3 &position)
{
    std::lock_guard<std::mutex> m(cameraMutex);
    Get()->SetPosition(position);
}
uint32 CameraWrapper::SubscribeOnChange(std::function<void(const ICamera &)> callback)
{
    std::lock_guard<std::mutex> m(cameraMutex);

    auto cameraSubId  = Get()->SubscribeOnChange(callback);
    auto wrapperSubId = subscribtionsIdPool_.getId();

    subscribtions_.insert({wrapperSubId, callback});
    subscribtionMap_.insert({wrapperSubId, cameraSubId});

    return wrapperSubId;
}
void CameraWrapper::UnsubscribeOnChange(uint32 id)
{
    std::lock_guard<std::mutex> m(cameraMutex);

    auto idIter = subscribtionMap_.find(id);

    if (idIter != subscribtionMap_.end())
    {
        auto cameraSubId = idIter->second;
        Get()->UnsubscribeOnChange(cameraSubId);

        subscribtions_.erase(id);
        subscribtionMap_.erase(id);
        subscribtionsIdPool_.releaseId(id);
    }
}

void CameraWrapper::notifySubscribersWhenCameraChange()
{
    for (auto &[_, function] : subscribtions_)
    {
        function(*Get());
    }
}

void CameraWrapper::moveSubscribtionsToCurrentCamera(ICamera &source, ICamera &target)
{
    for (const auto &[wrapperSubId, func] : subscribtions_)
    {
        auto idIter = subscribtionMap_.find(wrapperSubId);
        if (idIter != subscribtionMap_.end())
        {
            auto &cameraSubId = idIter->second;
            source.UnsubscribeOnChange(cameraSubId);
            cameraSubId = target.SubscribeOnChange(func);
        }
    }
}
}  // namespace GameEngine
