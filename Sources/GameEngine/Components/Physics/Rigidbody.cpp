#include "Rigidbody.h"

#include "BoxShape.h"
#include "CapsuleShape.h"
#include "CollisionShape.h"
#include "CylinderShape.h"
#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "Logger/Log.h"
#include "MeshShape.h"
#include "SphereShape.h"
#include "Terrain/TerrainShape.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string COMPONENT_STR             = "Rigidbody";
const std::string CSTR_MASS                 = "mass";
const std::string CSTR_IS_STATIC            = "isStatic";
const std::string CSTR_VELOCITY             = "velocity";
const std::string CSTR_ANGULAR_FACTOR       = "angularFactor";
const std::string CSTR_COLLISION_SHAPE      = "collisionShape";
const std::string CSTR_NO_CONCTACT_RESPONSE = "noConctactResponse";
}  // namespace

Rigidbody::Rigidbody(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(COMPONENT_STR, componentContext, gameObject)
    , mass(1.0f)
    , collisionShape_(nullptr)
    , updateRigidbodyOnTransformChange_(false)
{
    // clang-format off
    nameToTypeMap_ =
    {
        {BoxShape::name,     typeid(BoxShape).hash_code()},
        {TerrainShape::name, typeid(TerrainShape).hash_code()},
        {MeshShape::name,    typeid(MeshShape).hash_code()},
        {SphereShape::name,  typeid(SphereShape).hash_code()},
        {CapsuleShape::name, typeid(CapsuleShape).hash_code()},
        {CylinderShape::name, typeid(CylinderShape).hash_code()},
    };
    // clang-format on
}

void Rigidbody::CleanUp()
{
    if (worldTransformSubscriptionId_)
    {
        thisObject_.UnsubscribeOnWorldTransfromChange(*worldTransformSubscriptionId_);
    }

    if (rigidBodyId_)
    {
        componentContext_.physicsApi_.RemoveRigidBody(*rigidBodyId_);
        rigidBodyId_ = std::nullopt;
    }
}
void Rigidbody::OnStart()
{
    collisionShape_ = GetCollisionShape();

    if (not collisionShape_)
    {
        ERROR_LOG("Can not create Rigidbody without shape.");
        return;
    }

    auto maybeShapeId = collisionShape_->GetCollisionShapeId();

    if (not maybeShapeId)
    {
        ERROR_LOG("Shape not initilized!");
        return;
    }

    rigidBodyId_ = componentContext_.physicsApi_.CreateRigidbody(*maybeShapeId, thisObject_, rigidbodyPropierties, mass,
                                                                 updateRigidbodyOnTransformChange_);
    if (not rigidBodyId_)
    {
        ERROR_LOG("create rigidbody error.");
        return;
    }

    if (glm::length(velocity) > 0.0001f)
        componentContext_.physicsApi_.SetVelocityRigidbody(rigidBodyId_, velocity);

    componentContext_.physicsApi_.SetAngularFactor(rigidBodyId_, angularFactor);
    worldTransformSubscriptionId_ = thisObject_.SubscribeOnWorldTransfomChange(
        [this](const auto& transform)
        {
            if (not updateRigidbodyOnTransformChange_)
            {
                SetPosition(transform.GetPosition());
                SetRotation(transform.GetRotation());
                SetScale(transform.GetScale());
            }
        });

    for (auto& f : callWhenReady_)
    {
        f();
    }

    DEBUG_LOG("[" + thisObject_.GetName() + "] Rigidbody created. Id : " + std::to_string(rigidBodyId_));
}
void Rigidbody::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart, std::bind(&Rigidbody::OnStart, this));
}
bool Rigidbody::IsReady() const
{
    return rigidBodyId_.has_value();
}
Rigidbody& Rigidbody::SetMass(float m)
{
    mass = m;
    return *this;
}
Rigidbody& Rigidbody::SetIsStatic(bool is)
{
    isStaticObject = is;
    if (is)
    {
        SetMass(0.f);
        rigidbodyPropierties.insert(Physics::RigidbodyProperty::Static);
    }
    else
    {
        rigidbodyPropierties.erase(Physics::RigidbodyProperty::Static);
    }
    return *this;
}
Rigidbody& Rigidbody::SetCollisionShape(const std::string& shapeName)
{
    collisionShapeName = shapeName;
    return *this;
}
Rigidbody& Rigidbody::SetVelocity(const vec3& v)
{
    if (not rigidBodyId_)
    {
        velocity = v;
        return *this;
    }

    componentContext_.physicsApi_.SetVelocityRigidbody(*rigidBodyId_, v);
    return *this;
}
Rigidbody& Rigidbody::SetAngularFactor(float v)
{
    if (not rigidBodyId_)
    {
        angularFactor = vec3(v);
        return *this;
    }

    componentContext_.physicsApi_.SetAngularFactor(*rigidBodyId_, v);
    return *this;
}

Rigidbody& Rigidbody::SetAngularFactor(const vec3& v)
{
    if (not rigidBodyId_)
    {
        angularFactor = v;
        return *this;
    }

    componentContext_.physicsApi_.SetAngularFactor(*rigidBodyId_, v);
    return *this;
}

Rigidbody& Rigidbody::SetNoContactResponse(bool is)
{
    isNoConctactResponse = is;
    if (is)
    {
        rigidbodyPropierties.insert(Physics::RigidbodyProperty::NoContactResponse);
    }
    else
    {
        rigidbodyPropierties.erase(Physics::RigidbodyProperty::NoContactResponse);
    }
    return *this;
}
Rigidbody& Rigidbody::SetRotation(const DegreesVec3& rotation)
{
    if (not rigidBodyId_)
        return *this;

    componentContext_.physicsApi_.SetRotation(*rigidBodyId_, rotation.Radians());
    return *this;
}
Rigidbody& Rigidbody::SetRotation(const RadiansVec3& rotation)
{
    if (not rigidBodyId_)
        return *this;

    componentContext_.physicsApi_.SetRotation(*rigidBodyId_, rotation.value);
    return *this;
}
Rigidbody& Rigidbody::SetRotation(const Quaternion& rotation)
{
    if (not rigidBodyId_)
        return *this;

    componentContext_.physicsApi_.SetRotation(*rigidBodyId_, rotation);
    return *this;
}
Rigidbody& Rigidbody::SetRotation(const Rotation& rotation)
{
    SetRotation(rotation.value_);
    return *this;
}
Rigidbody& Rigidbody::SetPosition(const vec3& pos)
{
    if (not rigidBodyId_)
        return *this;

    componentContext_.physicsApi_.SetPosition(*rigidBodyId_, pos);
    return *this;
}
Rigidbody& Rigidbody::SetScale(const vec3& scale)
{
    if (not rigidBodyId_)
        return *this;

    if (collisionShape_)
    {
        collisionShape_->setScale(scale);
    }
    return *this;
}

void Rigidbody::Translate(const vec3& v)
{
    if (not rigidBodyId_)
        return;

    componentContext_.physicsApi_.Translate(*rigidBodyId_, v);
}
void Rigidbody::ApplyImpulse(const vec3& v)
{
    if (not rigidBodyId_)
        return;

    componentContext_.physicsApi_.ApplyImpulse(*rigidBodyId_, v);
}
void Rigidbody::IncreaseVelocity(const vec3& v)
{
    if (not rigidBodyId_)
        return;

    componentContext_.physicsApi_.IncreaseVelocityRigidbody(*rigidBodyId_, v);
}
void Rigidbody::SetAsVisualizatedObject()
{
    if (rigidBodyId_)
    {
        componentContext_.physicsApi_.enableVisualizatedRigidbody(*rigidBodyId_);
    }
}
Rigidbody& Rigidbody::callWhenReady(std::function<void()> f)
{
    if (IsReady())
    {
        f();
    }
    else
    {
        callWhenReady_.push_back(f);
    }

    return *this;
}
float Rigidbody::GetMass() const
{
    return mass;
}
bool Rigidbody::IsStatic() const
{
    return rigidbodyPropierties.contains(Physics::RigidbodyProperty::Static);
}
const std::string& Rigidbody::GetCollisionShapeType() const
{
    return collisionShapeName;
}
vec3 Rigidbody::GetVelocity() const
{
    if (not rigidBodyId_)
        return vec3(0.f);
    return *componentContext_.physicsApi_.GetVelocity(*rigidBodyId_);
}
vec3 Rigidbody::GetAngularFactor() const
{
    if (not rigidBodyId_)
        return {};

    return *componentContext_.physicsApi_.GetAngularFactor(*rigidBodyId_);
}

Quaternion Rigidbody::GetRotation() const
{
    if (not rigidBodyId_)
        return {};

    return *componentContext_.physicsApi_.GetRotation(*rigidBodyId_);
}

common::Transform Rigidbody::GetTransform() const
{
    if (not rigidBodyId_)
        return {};

    return *componentContext_.physicsApi_.GetTransfrom(*rigidBodyId_);
}

const std::optional<uint32>& Rigidbody::GetId() const
{
    return rigidBodyId_;
}

template <class T>
void Rigidbody::detectShape()
{
    auto shape = thisObject_.GetComponent<T>();

    if (shape)
    {
        detectedCollisionShapes_.insert({typeid(T).hash_code(), shape});
    }
}

CollisionShape* Rigidbody::GetCollisionShape()
{
    detectShape<BoxShape>();
    detectShape<TerrainShape>();
    detectShape<MeshShape>();
    detectShape<SphereShape>();
    detectShape<CapsuleShape>();
    detectShape<CylinderShape>();

    if (collisionShapeName.empty())
    {
        for (auto& pair : detectedCollisionShapes_)
        {
            return pair.second;
        }
        return nullptr;
    }

    auto shapeTypeIter = nameToTypeMap_.find(collisionShapeName);
    if (shapeTypeIter == nameToTypeMap_.end())
    {
        ERROR_LOG("Shape name " + collisionShapeName + " not found in nameToTypeMap");
        return nullptr;
    }

    auto shapeType         = shapeTypeIter->second;
    auto shapeDetectedIter = detectedCollisionShapes_.find(shapeType);
    if (shapeDetectedIter != detectedCollisionShapes_.end())
    {
        return shapeDetectedIter->second;
    }

    for (auto& pair : detectedCollisionShapes_)
    {
        return pair.second;
    }

    ERROR_LOG(thisObject_.GetName() + ". Shape type (" + collisionShapeName + ") is not found.");
    return nullptr;
}
void Rigidbody::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component = std::make_unique<Rigidbody>(componentContext, gameObject);

        float mass{1.f};
        ::Read(node.getChild(CSTR_MASS), mass);
        component->SetMass(mass);

        bool isStaticValue(true);
        ::Read(node.getChild(CSTR_IS_STATIC), isStaticValue);
        component->SetIsStatic(isStaticValue);

        bool isNoConctactResponseValue(false);
        ::Read(node.getChild(CSTR_NO_CONCTACT_RESPONSE), isNoConctactResponseValue);
        component->SetNoContactResponse(isNoConctactResponseValue);

        vec3 velocity(0.f);
        ::Read(node.getChild(CSTR_VELOCITY), velocity);
        component->SetVelocity(velocity);

        vec3 angularFactor(1.f);
        const auto& angularFactorNode = node.getChild(CSTR_ANGULAR_FACTOR);
        if (angularFactorNode and not angularFactorNode->value_.empty())
        {
            float angularFactorFloat{1.f};
            ::Read(angularFactorNode, angularFactorFloat);
            angularFactor = vec3(angularFactorFloat);
        }
        else
        {
            ::Read(angularFactorNode, angularFactor);
        }
        component->SetAngularFactor(angularFactor);

        std::string collisionShapeName;
        ::Read(node.getChild(CSTR_COLLISION_SHAPE), collisionShapeName);
        component->SetCollisionShape(collisionShapeName);
        return component;
    };
    regsiterComponentReadFunction(COMPONENT_STR, readFunc);
}
void Rigidbody::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});

    ::write(node.addChild(CSTR_MASS), mass);
    ::write(node.addChild(CSTR_IS_STATIC), isStaticObject);
    ::write(node.addChild(CSTR_NO_CONCTACT_RESPONSE), isNoConctactResponse);
    ::write(node.addChild(CSTR_COLLISION_SHAPE), collisionShapeName);
    ::write(node.addChild(CSTR_ANGULAR_FACTOR), angularFactor);
    ::write(node.addChild(CSTR_VELOCITY), velocity);
}
}  // namespace Components
}  // namespace GameEngine
