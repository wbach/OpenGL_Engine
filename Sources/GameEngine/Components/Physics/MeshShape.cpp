#include "MeshShape.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Resources/Models/Mesh.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/ResourceManager.h"

namespace GameEngine
{
namespace Components
{
ComponentsType MeshShape::type = ComponentsType::MeshShape;

MeshShape::MeshShape(const ComponentContext& componentContext, GameObject& gameObject)
    : CollisionShape(ComponentsType::MeshShape, componentContext, gameObject)
    , size_(1.f)
    , model_(nullptr)
{
}
void MeshShape::Update()
{
}
void MeshShape::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&MeshShape::OnAwake, this));
}
void MeshShape::OnAwake()
{
    if (model_ == nullptr)
    {
        return;
    }

    const auto& meshes = model_->GetMeshes();

    for (const auto& mesh : meshes)
    {
        auto data         = mesh.GetCMeshDataRef();
        collisionShapeId_ = componentContext_.physicsApi_.CreateMeshCollider(positionOffset_, data.positions_, data.indices_);
    }
}
void MeshShape::SetModel(Model* model)
{
    model_ = model;
}
}  // Components
}  // GameEngine
