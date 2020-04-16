#include "CameraWrapper.h"
#include <Utils/Mutex.hpp>

namespace GameEngine
{
namespace
{
std::mutex cameraMutex;
}
CameraWrapper::CameraWrapper()
    : camera_(&baseCamera_)
{
}

CameraWrapper::CameraWrapper(ICamera& camera)
    : camera_(&camera)
{
}

void CameraWrapper::Set(ICamera& camera)
{
    std::lock_guard<std::mutex> m(cameraMutex);
    camera_ = &camera;
}

ICamera* CameraWrapper::Get() const
{
    return camera_;
}

void CameraWrapper::Update()
{
    std::lock_guard<std::mutex> m(cameraMutex);
    camera_->CalculateInput();
    camera_->Move();
    camera_->UpdateMatrix();
}

void CameraWrapper::Move()
{
    std::lock_guard<std::mutex> m(cameraMutex);
    camera_->Move();
}

void CameraWrapper::CalculateInput()
{
    std::lock_guard<std::mutex> m(cameraMutex);
    camera_->CalculateInput();
}

void CameraWrapper::CalculateZoom(float lvl)
{
    std::lock_guard<std::mutex> m(cameraMutex);
    camera_->CalculateZoom(lvl);
}

void CameraWrapper::Lock()
{
    std::lock_guard<std::mutex> m(cameraMutex);
    camera_->Lock();
}

void CameraWrapper::Unlock()
{
    std::lock_guard<std::mutex> m(cameraMutex);
    camera_->Unlock();
}

void CameraWrapper::UpdateMatrix()
{
    std::lock_guard<std::mutex> m(cameraMutex);
    camera_->UpdateMatrix();
}

void CameraWrapper::InvertPitch()
{
    std::lock_guard<std::mutex> m(cameraMutex);
    camera_->InvertPitch();
}

float CameraWrapper::GetPitch() const
{
    std::lock_guard<std::mutex> m(cameraMutex);
    return camera_->GetPitch();
}

float CameraWrapper::GetYaw() const
{
    std::lock_guard<std::mutex> m(cameraMutex);
    return camera_->GetYaw();
}

float CameraWrapper::GetRoll() const
{
    std::lock_guard<std::mutex> m(cameraMutex);
    return camera_->GetRoll();
}

const vec3 &CameraWrapper::GetRotation() const
{
    std::lock_guard<std::mutex> m(cameraMutex);
    return camera_->GetRotation();
}

const vec3 &CameraWrapper::GetPosition() const
{
    std::lock_guard<std::mutex> m(cameraMutex);
    return camera_->GetPosition();
}

const vec3 &CameraWrapper::GetDirection() const
{
    std::lock_guard<std::mutex> m(cameraMutex);
    return camera_->GetDirection();
}

const mat4 &CameraWrapper::GetViewMatrix() const
{
    std::lock_guard<std::mutex> m(cameraMutex);
    return camera_->GetViewMatrix();
}

void CameraWrapper::LookAt(const vec3 &at)
{
    std::lock_guard<std::mutex> m(cameraMutex);
    camera_->LookAt(at);
}

void CameraWrapper::SetYaw(float yaw)
{
    std::lock_guard<std::mutex> m(cameraMutex);
    camera_->SetYaw(yaw);
}

void CameraWrapper::SetRoll(float roll)
{
    std::lock_guard<std::mutex> m(cameraMutex);
    camera_->SetRoll(roll);
}

void CameraWrapper::SetPitch(float pitch)
{
    std::lock_guard<std::mutex> m(cameraMutex);
    camera_->SetPitch(pitch);
}

void CameraWrapper::SetRotation(const vec3 &rotation)
{
    std::lock_guard<std::mutex> m(cameraMutex);
    camera_->SetRotation(rotation);
}

void CameraWrapper::SetPosition(const vec3 &position)
{
    std::lock_guard<std::mutex> m(cameraMutex);
    camera_->SetPosition(position);
}
uint32 CameraWrapper::SubscribeOnChange(std::function<void(const ICamera &)> callback)
{
    return camera_->SubscribeOnChange(callback);
}
void CameraWrapper::UnsubscribeOnChange(uint32 id)
{
    camera_->UnsubscribeOnChange(id);
}
}  // namespace GameEngine
