#include "MeshShape.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Resources/Models/Mesh.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/Models/ModelFactory.h"
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
        auto renderer = thisObject_.GetComponent<RendererComponent>();

        if (renderer == nullptr)
            return;

        model_ = renderer->GetModelWrapper().Get(GameEngine::L1);
    }

    const auto& meshes = model_->GetMeshes();

    for (const auto& mesh : meshes)
    {
        auto data         = mesh.GetCMeshDataRef();
        collisionShapeId_ = componentContext_.physicsApi_.CreateMeshCollider(
            positionOffset_, data.positions_, data.indices_, size_ / model_->GetScaleFactor());
    }
}
MeshShape& MeshShape::SetModel(Model* model)
{
    model_ = model;
    modelFileName_ = model->GetFileName();
    return *this;
}
MeshShape& MeshShape::SetModel(const std::string& filename)
{
    model_ = GameEngine::LoadModel(&componentContext_.resourceManager_, filename);
    return *this;
}
MeshShape& MeshShape::SetSize(float size)
{
    size_ = size;
    return *this;
}
const std::string& MeshShape::GetModelFileName() const
{
    return modelFileName_;
}
}  // namespace Components
}  // namespace GameEngine
