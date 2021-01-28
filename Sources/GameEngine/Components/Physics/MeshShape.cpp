#include "MeshShape.h"

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
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
const std::string MeshShape::name = "MeshShape";

MeshShape::MeshShape(ComponentContext& componentContext, GameObject& gameObject)
    : CollisionShape(typeid(MeshShape).hash_code(), componentContext, gameObject)
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
            positionOffset_, data.positions_, data.indices_, size_ / model_->getBoundingBox().maxScale());
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
void MeshShape::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
        auto component = std::make_unique<MeshShape>(componentContext, gameObject);

        vec3 positionOffset(0.f);
        ::Read(node.getChild(CSTR_POSITION_OFFSET), positionOffset);
        component->SetPostionOffset(positionOffset);

        float size(1.f);
        ::Read(node.getChild(CSTR_SIZE), size);
        component->SetSize(size);

        std::string model;
        ::Read(node.getChild(CSTR_MODEL_FILE_NAME), size);
        if (not model.empty())
        {
            component->SetModel(model);
        }

        return component;
    };
    regsiterComponentReadFunction(MeshShape::name, readFunc);
}
void MeshShape::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, MeshShape::name});

    ::write(node.addChild(CSTR_POSITION_OFFSET), GetPositionOffset());
    ::write(node.addChild(CSTR_SIZE), GetSize());
}
}  // namespace Components
}  // namespace GameEngine
