#include "MeshShape.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
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

MeshShape::MeshShape(ComponentContext& componentContext, GameObject& gameObject)
    : CollisionShape(ComponentsType::MeshShape, componentContext, gameObject)
    , size_(1.f)
    , model_(nullptr)
{
}

void MeshShape::CleanUp()
{
    // Render comonent loading model
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
    if (not model_)
    {
        auto renderer = thisObject_.GetComponent<RendererComponent>();

        if (not renderer)
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
    return *this;
}
MeshShape& MeshShape::SetModel(const File& filename)
{
    model_ = componentContext_.resourceManager_.LoadModel(filename);
    return *this;
}
const Model* MeshShape::GetModel() const
{
	return model_;
}
MeshShape& MeshShape::SetSize(float size)
{
    size_ = size;
    return *this;
}
}  // namespace Components
}  // namespace GameEngine
