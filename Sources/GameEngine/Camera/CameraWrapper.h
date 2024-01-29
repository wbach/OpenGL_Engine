#pragma once
#include <Utils/IdPool.h>

#include <memory>

#include "Camera.h"

namespace GameEngine
{
class CameraWrapper
{
public:
    CameraWrapper();
    CameraWrapper(ICamera& camera);

    IdType add(std::unique_ptr<ICamera>);
    IdType addAndSet(std::unique_ptr<ICamera>);
    void remove(IdType);

    void set(IdType);
    void pop();

    void Update();

    bool IsLocked() const;
    void Lock();
    void Unlock();

    void UpdateMatrix();

    float GetPitch() const;
    float GetYaw() const;

    const Rotation& GetRotation() const;
    const vec3& GetPosition() const;
    const vec3& GetDirection() const;

    const mat4& GetTranslationMatrix() const;
    const mat4& GetRotationMatrix() const;
    const mat4& GetViewMatrix() const;

    void LookAt(const vec3&);
    void SetYaw(float yaw);
    void SetPitch(float pitch);
    void SetRotation(const Rotation&);
    void SetPosition(const vec3&);

    IdType SubscribeOnChange(std::function<void(const ICamera&)>);
    void UnsubscribeOnChange(uint32);

private:
    void notifySubscribersWhenCameraChange();
    ICamera* Get() const;
    void setCamera(ICamera*);
    void removeCamera(ICamera*);
    void moveSubscribtionsToCurrentCamera(ICamera&, ICamera&);

private:
    Camera baseCamera_;
    Utils::IdPool camerasIdPool_;
    Utils::IdPool subscribtionsIdPool_;
    std::vector<ICamera*> setCameras_;
    std::unordered_map<IdType, std::unique_ptr<ICamera>> avaiableCameras_;
    std::unordered_map<IdType, std::function<void(const ICamera&)>> subscribtions_;
    std::unordered_map<IdType, IdType> subscribtionMap_;
};
}  // namespace GameEngine
