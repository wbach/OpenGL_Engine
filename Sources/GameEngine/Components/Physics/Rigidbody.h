#pragma once
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
    struct Params
    {
        std::optional<vec3> velocity_;
        std::optional<vec3> angularFactor_;
    };

public:
    Rigidbody(ComponentContext& componentContext, GameObject& gameObject);
    ~Rigidbody() override;
    void CleanUp() override;
    void ReqisterFunctions() override;

public:
    bool IsReady() const;
    Rigidbody& SetMass(float mass);
    Rigidbody& SetIsStatic(bool is);
    Rigidbody& SetCollisionShape(ComponentsType shapeType);
    Rigidbody& SetVelocity(const vec3& velocity);
    Rigidbody& SetAngularFactor(float);
    Rigidbody& SetAngularFactor(const vec3&);
    Rigidbody& SetRotation(const DegreesVec3&);
    Rigidbody& SetRotation(const RadiansVec3&);
    Rigidbody& SetRotation(const Quaternion&);
    Rigidbody& SetPosition(const vec3&);

    void ApplyImpulse(const vec3&);
    void IncreaseVelocity(const vec3& v);
    void SetAsVisualizatedObject();

    float GetMass() const;
    bool IsStatic() const;
    ComponentsType GetCollisionShapeType() const;
    vec3 GetVelocity() const;
    vec3 GetAngularFactor() const;
    Quaternion GetRotation() const;
    common::Transform GetTransform() const;

    Params& InputParams();
    const Params& InputParams() const;

private:
    void OnAwake();
    CollisionShape* GetCollisionShape();
    bool isShapeTypeValid(ComponentsType shapeType);

private:
    std::optional<uint32> rigidBodyId_;
    std::optional<ComponentsType> shapeType_;
    CollisionShape* collisionShape_;

    float mass_    = 1.f;
    bool isStatic_ = false;

    Params inputParams_;

    template <class T>
    void detectShape();
    std::unordered_map<ComponentsType, CollisionShape*> detectedCollisionShapes_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
