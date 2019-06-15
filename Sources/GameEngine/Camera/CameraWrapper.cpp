#include "CameraWrapper.h"
#include <Utils/Mutex.hpp>
#include "Camera.h"

namespace GameEngine
{
namespace
{
std::mutex cameraMutex;
}
CameraWrapper::CameraWrapper()
    : camera_(new BaseCamera(vec3(0, 5, 5), vec3(0, 0, 0)))
{
}

CameraWrapper::CameraWrapper(std::unique_ptr<ICamera> camera)
    : camera_(std::move(camera))
{
}

void CameraWrapper::Set(std::unique_ptr<ICamera> camera)
{
    std::lock_guard<std::mutex> m(cameraMutex);
    camera_.reset();
    camera_ = std::move(camera);
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
}  // namespace GameEngine
