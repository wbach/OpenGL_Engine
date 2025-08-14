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
const std::string MeshShape::name          = "MeshShape";
const std::string CSTR_AUTO_OPTIMIZE_MESH  = "autoOptimize";
const std::string CSTR_MODEL_NORMALIZATION = "modelNormalization";
const std::string CSTR_MESH_OPTIMIZE       = "meshOptimize";

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
        DEBUG_LOG("No dedicated model, try get from RendererComponent");
        auto renderer = thisObject_.GetComponent<RendererComponent>();

        if (not renderer)
            return;

        model_ = renderer->GetModelWrapper().Get(GameEngine::L1);
    }

    if (not model_)
    {
        DEBUG_LOG("Model from RendererComponent also not found return");
        return;
    }
    DEBUG_LOG("Model file used : " + model_->GetFile().GetFilename());

    auto scale = calculateScale(thisObject_.GetWorldTransform().GetScale());
    if (auto modelRawData = model_->getModelRawData())
    {
        collisionShapeId_ = componentContext_.physicsApi_.CreateMeshCollider(positionOffset, modelRawData->positions_,
                                                                             modelRawData->indices_, scale, autoOptimize_);
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
    model_                 = componentContext_.resourceManager_.LoadModel(filename, loadingParameters_);
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
    return size_ * scale * model_->getNormalizedFactor();
}
void MeshShape::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component = std::make_unique<MeshShape>(componentContext, gameObject);

        vec3 positionOffset(0.f);
        ::Read(node.getChild(CSTR_POSITION_OFFSET), positionOffset);
        component->SetPostionOffset(positionOffset);

        float size(1.f);
        ::Read(node.getChild(CSTR_SIZE), size);
        component->SetSize(size);

        auto filenameNode = node.getChild(CSTR_MODEL_FILE_NAME);
        if (filenameNode)
        {
            ::Read(filenameNode->getChild(CSTR_FILE_NAME), component->requstedModelFileName_);

            auto modelNormalization = filenameNode->getChild(CSTR_MODEL_NORMALIZATION);
            if (modelNormalization)
            {
                component->loadingParameters_.modelNormalization =
                    Utils::StringToBool(modelNormalization->value_) ? ModelNormalization::normalized : ModelNormalization::none;
            }
            auto meshOptimize = filenameNode->getChild(CSTR_MESH_OPTIMIZE);
            if (meshOptimize)
            {
                component->loadingParameters_.meshOptimize =
                    Utils::StringToBool(meshOptimize->value_) ? MeshOptimize::optimized : MeshOptimize::none;
            }

            if (not component->requstedModelFileName_.empty())
            {
                DEBUG_LOG("Set collider model :" + component->requstedModelFileName_);
                component->SetModel(component->requstedModelFileName_);
            }
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
    ::write(node.addChild(CSTR_AUTO_OPTIMIZE_MESH), Utils::BoolToString(autoOptimize_));
    auto& modelNode = node.addChild(CSTR_MODEL_FILE_NAME);
    ::write(modelNode.addChild(CSTR_FILE_NAME), requstedModelFileName_);
    modelNode.addChild(CSTR_MODEL_NORMALIZATION,
                       Utils::BoolToString(loadingParameters_.modelNormalization == ModelNormalization::normalized));
    modelNode.addChild(CSTR_MESH_OPTIMIZE, Utils::BoolToString(loadingParameters_.meshOptimize == MeshOptimize::optimized));
}
}  // namespace Components
}  // namespace GameEngine
