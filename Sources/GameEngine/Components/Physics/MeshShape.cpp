#include "MeshShape.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>

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
const std::string MeshShape::name    = "MeshShape";
const std::string CSTR_AUTO_OPTIMIZE_MESH = "autoOptimize";

MeshShape::MeshShape(ComponentContext& componentContext, GameObject& gameObject)
    : CollisionShape(typeid(MeshShape).hash_code(), componentContext, gameObject)
    , size_(1.f)
    , model_(nullptr)
    , autoOptimize_{false}
{
}

void MeshShape::CleanUp()
{
    // Render comonent loading model
    CollisionShape::CleanUp();
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

    if (not model_)
        return;

    const auto& meshes = model_->GetMeshes();
    auto scale = calculateScale(thisObject_.GetWorldTransform().GetScale());

    if (meshes.size() == 1)
    {
        auto data         = meshes.front().GetCMeshDataRef();
        collisionShapeId_ = componentContext_.physicsApi_.CreateMeshCollider(positionOffset_, data.positions_,
                                                                             data.indices_, scale, autoOptimize_);
    }
    else
    {
        size_t indiciesSize = 0;
        size_t dataSize = 0;
        for (const auto& mesh : meshes)
        {
            const auto& meshData = mesh.GetCMeshDataRef();
            indiciesSize += meshData.indices_.size();
            dataSize += meshData.positions_.size();
        }

        std::vector<float> data;
        IndicesVector indicies;
        data.reserve(dataSize);
        indicies.reserve(indiciesSize);

        for (const auto& mesh : meshes)
        {
            const auto& meshData = mesh.GetCMeshDataRef();
            indicies.reserve(meshData.indices_.size());

            for (auto& i : meshData.indices_)
            {
                indicies.push_back(i + (data.size() / 3.f));
            }
            data.insert(std::end(data), std::begin(meshData.positions_), std::end(meshData.positions_));
        }
        collisionShapeId_ = componentContext_.physicsApi_.CreateMeshCollider(positionOffset_, data, indicies, scale, autoOptimize_);
    }
}
void MeshShape::setScale(const vec3& scale)
{
    CollisionShape::setScale(calculateScale(scale));
}
MeshShape& MeshShape::SetModel(Model* model)
{
    model_ = model;
    return *this;
}
MeshShape& MeshShape::SetModel(const File& filename)
{
    requstedModelFileName_ = filename.GetInitValue();
    model_                 = componentContext_.resourceManager_.LoadModel(filename);
    return *this;
}
void MeshShape::autoOptimize()
{
    autoOptimize_ = true;
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
vec3 MeshShape::calculateScale(const vec3& scale) const
{
    const auto& meshes = model_->GetMeshes();

    if (meshes.empty())
        return vec3(1.f);

    const auto& meshScale = meshes.front().GetNormalizedScale();
    return size_ * scale * meshScale;
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
        ::Read(node.getChild(CSTR_MODEL_FILE_NAME), model);
        if (not model.empty())
        {
            component->SetModel(model);
        }

        bool autoOptimize_{false};
        ::Read(node.getChild(CSTR_AUTO_OPTIMIZE_MESH), autoOptimize_);
        if (autoOptimize_)
        {
            component->autoOptimize();
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
    ::write(node.addChild(CSTR_MODEL_FILE_NAME), requstedModelFileName_);
    ::write(node.addChild(CSTR_AUTO_OPTIMIZE_MESH), Utils::BoolToString(autoOptimize_));
}
}  // namespace Components
}  // namespace GameEngine
