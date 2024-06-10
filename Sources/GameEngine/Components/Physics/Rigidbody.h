#pragma once
#include <optional>
#include <unordered_map>

#include "Common/Transform.h"
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Physics/ShapeType.h"
#include <GameEngine/Physics/IPhysicsApi.h>

namespace GameEngine
{
namespace Components
{
class CollisionShape;

class Rigidbody : public BaseComponent
{
public:
    struct Params
    {
        std::optional<vec3> velocity_;
        std::optional<vec3> angularFactor_;
    };

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
    Rigidbody &SetNoContactResponse(bool);
    Rigidbody& SetRotation(const DegreesVec3&);
    Rigidbody& SetRotation(const RadiansVec3&);
    Rigidbody& SetRotation(const Quaternion&);
    Rigidbody& SetRotation(const Rotation&);
    Rigidbody& SetPosition(const vec3&);
    Rigidbody& SetScale(const vec3&);

    void ApplyImpulse(const vec3&);
    void IncreaseVelocity(const vec3& v);
    void SetAsVisualizatedObject();

    float GetMass() const;
    bool IsStatic() const;
    const std::string& GetCollisionShapeType() const;
    vec3 GetVelocity() const;
    vec3 GetAngularFactor() const;
    Quaternion GetRotation() const;
    common::Transform GetTransform() const;

    Params& InputParams();
    const Params& InputParams() const;

     const std::optional<uint32>& GetId() const;

private:
    void OnStart();
    CollisionShape* GetCollisionShape();

private:
    std::optional<uint32> rigidBodyId_;
    std::string shapeName_;
    CollisionShape* collisionShape_;

    float mass_    = 1.f;
    Params inputParams_;
    Params tmpParams_;

    template <class T>
    void detectShape();
    std::unordered_map<size_t, CollisionShape*> detectedCollisionShapes_;
    std::unordered_map<std::string, size_t> nameToTypeMap_;

    bool updateRigidbodyOnTransformChange_;
    std::optional<IdType> worldTransformSubscriptionId_;

    Physics::RigidbodyProperties rigidbodyPropierties;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
