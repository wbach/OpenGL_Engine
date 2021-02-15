#include "Rigidbody.h"

#include "BoxShape.h"
#include "CapsuleShape.h"
#include "CollisionShape.h"
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
const std::string COMPONENT_STR        = "Rigidbody";
const std::string CSTR_MASS            = "mass";
const std::string CSTR_IS_STATIC       = "isStatic";
const std::string CSTR_VELOCITY        = "velocity";
const std::string CSTR_ANGULAR_FACTOR  = "angularFactor";
const std::string CSTR_COLLISION_SHAPE = "collisionShape";
}  // namespace

Rigidbody::Rigidbody(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(Rigidbody).hash_code(), componentContext, gameObject)
    , collisionShape_(nullptr)
    , mass_(1.0f)
    , isStatic_(false)
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
    };
    // clang-format on
}

Rigidbody::~Rigidbody()
{
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
        return;
    }

    DEBUG_LOG("CreateRigidbody : " + thisObject_.GetName());
    auto rigidBodyId = componentContext_.physicsApi_.CreateRigidbody(*maybeShapeId, thisObject_, mass_, isStatic_,
                                                                     updateRigidbodyOnTransformChange_);

    if (rigidBodyId == 0)
    {
        ERROR_LOG("create rigidbody error.");
        return;
    }

    rigidBodyId_ = rigidBodyId;

    if (inputParams_.velocity_)
        componentContext_.physicsApi_.SetVelocityRigidbody(rigidBodyId, *inputParams_.velocity_);

    if (inputParams_.angularFactor_)
        componentContext_.physicsApi_.SetAngularFactor(rigidBodyId, *inputParams_.angularFactor_);

    worldTransformSubscriptionId_ = thisObject_.SubscribeOnWorldTransfomChange([this](const auto& transform) {
        if (not updateRigidbodyOnTransformChange_)
        {
            SetPosition(transform.GetPosition());
            SetRotation(transform.GetRotation());
            SetScale(transform.GetScale());
        }
    });
}
void Rigidbody::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart, std::bind(&Rigidbody::OnStart, this));
}
bool Rigidbody::IsReady() const
{
    return rigidBodyId_.has_value();
}
Rigidbody& Rigidbody::SetMass(float mass)
{
    mass_ = mass;
    return *this;
}
Rigidbody& Rigidbody::SetIsStatic(bool is)
{
    isStatic_ = is;
    if (is)
    {
        SetMass(0.f);
    }
    return *this;
}
Rigidbody& Rigidbody::SetCollisionShape(const std::string& shapeName)
{
    shapeName_ = shapeName;
    return *this;
}
Rigidbody& Rigidbody::SetVelocity(const vec3& velocity)
{
    if (not rigidBodyId_)
        return *this;

    componentContext_.physicsApi_.SetVelocityRigidbody(*rigidBodyId_, velocity);
    return *this;
}
Rigidbody& Rigidbody::SetAngularFactor(float v)
{
    if (not rigidBodyId_)
        return *this;

    componentContext_.physicsApi_.SetAngularFactor(*rigidBodyId_, v);
    return *this;
}

Rigidbody& Rigidbody::SetAngularFactor(const vec3& angularFactor)
{
    if (not rigidBodyId_)
        return *this;

    componentContext_.physicsApi_.SetAngularFactor(*rigidBodyId_, angularFactor);
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
        componentContext_.physicsApi_.setVisualizatedRigidbody(*rigidBodyId_);
    }
}
float Rigidbody::GetMass() const
{
    return mass_;
}
bool Rigidbody::IsStatic() const
{
    return isStatic_;
}
const std::string& Rigidbody::GetCollisionShapeType() const
{
    return shapeName_;
}
vec3 Rigidbody::GetVelocity() const
{
    if (not rigidBodyId_)
        return {};
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

Rigidbody::Params& Rigidbody::InputParams()
{
    return inputParams_;
}

const Rigidbody::Params& Rigidbody::InputParams() const
{
    return inputParams_;
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

    if (shapeName_.empty())
    {
        WARNING_LOG("Shape type is not set, searching another shape component.");
        for (auto& pair : detectedCollisionShapes_)
        {
            WARNING_LOG("Shape type is found, shape type is set to : " + std::to_string(pair.first));
            return pair.second;
        }
        return nullptr;
    }

    auto shapeTypeIter = nameToTypeMap_.find(shapeName_);
    if (shapeTypeIter == nameToTypeMap_.end())
    {
        ERROR_LOG("Shape name " + shapeName_ + " not found in nameToTypeMap");
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
        WARNING_LOG("Requsted shape (" + shapeName_ +
                    ") not found but another is detected, shape type is set to : " + std::to_string(shapeType));
        return pair.second;
    }

    ERROR_LOG("Shape type (" + shapeName_ + ") is not found.");
    return nullptr;
}
void Rigidbody::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
        auto component = std::make_unique<Rigidbody>(componentContext, gameObject);

        float mass{1.f};
        ::Read(node.getChild(CSTR_MASS), mass);
        component->SetMass(mass);

        bool isStatic(true);
        ::Read(node.getChild(CSTR_IS_STATIC), isStatic);
        component->SetIsStatic(isStatic);

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

    ::write(node.addChild(CSTR_MASS), GetMass());
    ::write(node.addChild(CSTR_IS_STATIC), IsStatic());
    ::write(node.addChild(CSTR_COLLISION_SHAPE), GetCollisionShapeType());

    auto angularFactor = InputParams().angularFactor_;
    if (angularFactor)
    {
        ::write(node.addChild(CSTR_ANGULAR_FACTOR), *angularFactor);
    }

    auto velocty = InputParams().velocity_;
    if (velocty)
    {
        ::write(node.addChild(CSTR_VELOCITY), *velocty);
    }
}
}  // namespace Components
}  // namespace GameEngine
