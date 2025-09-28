#pragma once
#include <GameEngine/Physics/IPhysicsApi.h>

#include <optional>
#include <unordered_map>

#include "Common/Transform.h"
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Physics/ShapeType.h"

namespace GameEngine
{
namespace Components
{
class CollisionShape;

class Rigidbody : public BaseComponent
{
public:
    float mass{1.f};
    bool isStaticObject{true};
    bool isNoConctactResponse{false};
    vec3 velocity{0.f, 0.f, 0.f};
    vec3 angularFactor{1.f, 1.f, 1.f};
    std::string collisionShapeName;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_STRING(collisionShapeName)
        FIELD_FLOAT(mass)
        FIELD_BOOL(isStaticObject)
        FIELD_BOOL(isNoConctactResponse)
        FIELD_VECTOR3F(velocity)
        FIELD_VECTOR3F(angularFactor)
    END_FIELDS()
    // clang-format on

public:
    Rigidbody(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;

public:
    bool IsReady() const;
    Rigidbody& SetMass(float mass);
    Rigidbody& SetIsStatic(bool is);
    Rigidbody& SetCollisionShape(const std::string&);
    Rigidbody& SetVelocity(const vec3& velocity);
    Rigidbody& SetAngularFactor(float);
    Rigidbody& SetAngularFactor(const vec3&);
    Rigidbody& SetNoContactResponse(bool);
    Rigidbody& SetRotation(const DegreesVec3&);
    Rigidbody& SetRotation(const RadiansVec3&);
    Rigidbody& SetRotation(const Quaternion&);
    Rigidbody& SetRotation(const Rotation&);
    Rigidbody& SetPosition(const vec3&);
    Rigidbody& SetScale(const vec3&);

    void Translate(const vec3&);

    void ApplyImpulse(const vec3&);
    void IncreaseVelocity(const vec3& v);
    void SetAsVisualizatedObject();
    Rigidbody& callWhenReady(std::function<void()>);

    float GetMass() const;
    bool IsStatic() const;
    const std::string& GetCollisionShapeType() const;
    vec3 GetVelocity() const;
    vec3 GetAngularFactor() const;
    Quaternion GetRotation() const;
    common::Transform GetTransform() const;

    const std::optional<uint32>& GetId() const;

private:
    void Init();
    CollisionShape* GetCollisionShape();

private:
    std::optional<uint32> rigidBodyId_;
    CollisionShape* collisionShape_;

    template <class T>
    void detectShape();
    std::unordered_map<size_t, CollisionShape*> detectedCollisionShapes_;
    std::unordered_map<std::string, size_t> nameToTypeMap_;

    bool updateRigidbodyOnTransformChange_;
    std::optional<IdType> worldTransformSubscriptionId_;

    Physics::RigidbodyProperties rigidbodyPropierties;

    std::vector<std::function<void()>> callWhenReady_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
