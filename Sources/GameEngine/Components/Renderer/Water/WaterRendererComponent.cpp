#include "WaterRendererComponent.h"

#include <Common/Transform.h>
#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>
#include <Utils/TreeNode.h>

#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Objects/Water/MeshWaterFactory.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Models/BoundingBox.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
namespace Components
{
namespace
{
constexpr char CSTR_COLOR[]                = "color";
constexpr char CSTR_TILED_VALUE[]          = "tiledValue";
constexpr char CSTR_PLANE_WAVE_SPEED[]     = "planeWaveSpeed";
constexpr char CSTR_WAVE_SPEED[]           = "waveSpeed";
constexpr char CSTR_WAVE_AMPLITUDE[]       = "waveAmplitude";
constexpr char CSTR_WAVE_FREQUENCY[]       = "waveFrequency";
constexpr char CSTR_MAX_DEPTH_VISIBILITY[] = "maxDepthVisibility";
constexpr char CSTR_DEPTH_BLEND_SCALE[]    = "depthBlendScale";
constexpr char CSTR_DUDV_MAP[]             = "dudv";
constexpr char CSTR_NORMAL_MAP[]           = "normalMap";
constexpr char CSTR_MESH_RESOLUTION[]      = "meshResolution";
}  // namespace

WaterRendererComponent::WaterRendererComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<WaterRendererComponent>(), componentContext, gameObject)
    , onPlaneWaveSpeed(.1f)
    , waveSpeed(.1f)
    , waterColor(Utils::RGBtoFloat(0.f, 44.f, 82.f), 1.f)
    , meshResolution{32}
    , moveFactor_(0)
    , waveMove_(0)
    , normalMap_{nullptr}
    , dudvMap_{nullptr}
    , isSubscribed_(false)
    , modelBoundingBox_(BoundingBox::NumericLimits::MaxOppositeValues)
{
}
WaterRendererComponent::~WaterRendererComponent()
{
}
void WaterRendererComponent::CleanUp()
{
    UnSubscribe();

    for (auto model : modelWrapper_.PopModels())
    {
        componentContext_.resourceManager_.ReleaseModel(*model);
    }

    if (onTransformChangeSubscribtion_)
    {
        thisObject_.UnsubscribeOnWorldTransfromChange(*onTransformChangeSubscribtion_);
        onTransformChangeSubscribtion_.reset();
    }
    if (perObjectUpdateBuffer_)
    {
        componentContext_.gpuResourceLoader_.AddObjectToRelease(std::move(perObjectUpdateBuffer_));
        perObjectUpdateBuffer_.reset();
    }

    DeleteTextures();
}
void WaterRendererComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&WaterRendererComponent::OnAwake, this));
}
void WaterRendererComponent::Reload()
{
    CleanUp();
    OnAwake();
}
const vec4& WaterRendererComponent::GetWaterColor() const
{
    return waterColor;
}
float WaterRendererComponent::moveFactor() const
{
    return moveFactor_;
}
float WaterRendererComponent::waveMoveFactor() const
{
    return waveMove_;
}
float WaterRendererComponent::increaseFactors(float deltaTime)
{
    moveFactor_ += onPlaneWaveSpeed * deltaTime;
    moveFactor_ = fmodf(moveFactor_, 1.f);

    waveMove_ += deltaTime * waveSpeed;
    // if (waveMove_ > 2.f * M_PI)
    //     waveMove_ -= 2.f * M_PI;

    return moveFactor_;
}
const Texture* WaterRendererComponent::GetDudvTexture() const
{
    return dudvMap_;
}
const Texture* WaterRendererComponent::GetNormalTexture() const
{
    return normalMap_;
}
WaterRendererComponent& WaterRendererComponent::LoadTextures(const File& dudv, const File& normal)
{
    TextureParameters paramters;
    paramters.mimap  = GraphicsApi::TextureMipmap::LINEAR;
    paramters.filter = GraphicsApi::TextureFilter::LINEAR;

    normalMap_ = componentContext_.resourceManager_.GetTextureLoader().LoadTexture(normal, paramters);
    dudvMap_   = componentContext_.resourceManager_.GetTextureLoader().LoadTexture(dudv, paramters);

    return *this;
}
GraphicsApi::ID WaterRendererComponent::getPerObjectUpdateBufferId() const
{
    if (perObjectUpdateBuffer_)
        return perObjectUpdateBuffer_->GetGraphicsObjectId();

    return std::nullopt;
}
WaterRendererComponent& WaterRendererComponent::SetWaveSpeed(float speed)
{
    waveSpeed = speed;
    return *this;
}
float WaterRendererComponent::GetWaveSpeed() const
{
    return waveSpeed;
}
void WaterRendererComponent::SetWaterColor(const vec4& color)
{
    waterColor = color;
}
void WaterRendererComponent::OnAwake()
{
    LOG_DEBUG << "OnAwake";
    if (not dudvMap.empty() and not normalMap.empty())
    {
        LoadTextures(dudvMap, normalMap);
    }

    if (not isSubscribed_)
    {
        LOG_DEBUG << "Subscribe";
        componentContext_.renderersManager_.Subscribe(&thisObject_);
        isSubscribed_ = true;
    }

    if (not perObjectUpdateBuffer_)
    {
        perObjectUpdateBuffer_ =
            std::make_unique<ShaderBufferObject<PerObjectUpdate>>(componentContext_.graphicsApi_, PER_OBJECT_UPDATE_BIND_LOCATION);
        updatePerObjectUpdateBuffer(thisObject_.GetWorldTransform());
    }

    if (not onTransformChangeSubscribtion_)
    {
        onTransformChangeSubscribtion_ = thisObject_.SubscribeOnWorldTransfomChange(
            [this](const common::Transform& transfrom)
            {
                createBoundingBoxes();
                updatePerObjectUpdateBuffer(transfrom);
            });
    }

    if (meshResolution > 0 and not modelWrapper_.Get(LevelOfDetail::L1))
    {
        createModel();
    }
    else
    {
        LOG_DEBUG << "Model already created. or meshResolution <= 0";
    }
}

void WaterRendererComponent::createModel()
{
    MeshWaterFactory meshFactory;
    auto model    = meshFactory.CreateAsSingleTile(componentContext_.graphicsApi_, meshResolution);
    auto modelPtr = model.get();
    componentContext_.resourceManager_.AddModel(std::move(model));
    modelWrapper_.Add(modelPtr, LevelOfDetail::L1);
    createBoundingBoxes();
}

void WaterRendererComponent::UnSubscribe()
{
    if (isSubscribed_)
    {
        componentContext_.renderersManager_.UnSubscribe(&thisObject_);
        isSubscribed_ = false;
    }
}
void WaterRendererComponent::updatePerObjectUpdateBuffer(const common::Transform& transfrom)
{
    if (not perObjectUpdateBuffer_)
        return;

    perObjectUpdateBuffer_->GetData().TransformationMatrix =
        componentContext_.graphicsApi_.PrepareMatrixToLoad(transfrom.CalculateCurrentMatrix());

    if (not perObjectUpdateBuffer_->GetGraphicsObjectId())
    {
        componentContext_.gpuResourceLoader_.AddObjectToGpuLoadingPass(*perObjectUpdateBuffer_);
    }
    else
    {
        componentContext_.gpuResourceLoader_.AddObjectToUpdateGpuPass(*perObjectUpdateBuffer_);
    }
}
void WaterRendererComponent::DeleteTextures()
{
    if (normalMap_)
    {
        componentContext_.resourceManager_.GetTextureLoader().DeleteTexture(*normalMap_);
        normalMap_ = nullptr;
    }
    if (dudvMap_)
    {
        componentContext_.resourceManager_.GetTextureLoader().DeleteTexture(*dudvMap_);
        dudvMap_ = nullptr;
    }
}
void WaterRendererComponent::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component = std::make_unique<WaterRendererComponent>(componentContext, gameObject);

        std::string dudvMap, normalMap;
        ::Read(node.getChild(CSTR_COLOR), component->waterColor);
        ::Read(node.getChild(CSTR_DUDV_MAP), dudvMap);
        ::Read(node.getChild(CSTR_NORMAL_MAP), normalMap);
        ::Read(node.getChild(CSTR_TILED_VALUE), component->tiledValue);
        ::Read(node.getChild(CSTR_MESH_RESOLUTION), component->meshResolution);
        ::Read(node.getChild(CSTR_WAVE_SPEED), component->waveSpeed);
        ::Read(node.getChild(CSTR_WAVE_FREQUENCY), component->waveFrequency);
        ::Read(node.getChild(CSTR_WAVE_AMPLITUDE), component->waveAmplitude);
        ::Read(node.getChild(CSTR_PLANE_WAVE_SPEED), component->onPlaneWaveSpeed);
        ::Read(node.getChild(CSTR_DEPTH_BLEND_SCALE), component->depthBlendScale);
        ::Read(node.getChild(CSTR_MAX_DEPTH_VISIBILITY), component->maxDepthVisibility);

        component->dudvMap   = dudvMap;
        component->normalMap = normalMap;

        return component;
    };

    regsiterComponentReadFunction(GetComponentType<WaterRendererComponent>(), readFunc);
}
void WaterRendererComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});

    ::write(node.addChild(CSTR_COLOR), GetWaterColor());
    ::write(node.addChild(CSTR_DUDV_MAP), dudvMap.GetDataRelativePath());
    ::write(node.addChild(CSTR_NORMAL_MAP), normalMap.GetDataRelativePath());
    ::write(node.addChild(CSTR_TILED_VALUE), tiledValue);
    ::write(node.addChild(CSTR_MESH_RESOLUTION), meshResolution);
    ::write(node.addChild(CSTR_WAVE_SPEED), GetWaveSpeed());
    ::write(node.addChild(CSTR_WAVE_FREQUENCY), waveFrequency);
    ::write(node.addChild(CSTR_WAVE_AMPLITUDE), waveAmplitude);
    ::write(node.addChild(CSTR_PLANE_WAVE_SPEED), onPlaneWaveSpeed);
    ::write(node.addChild(CSTR_MAX_DEPTH_VISIBILITY), maxDepthVisibility);
    ::write(node.addChild(CSTR_DEPTH_BLEND_SCALE), depthBlendScale);
}
Model* WaterRendererComponent::GetModel()
{
    return modelWrapper_.Get(LevelOfDetail::L1);
}
void WaterRendererComponent::createBoundingBoxes()
{
    auto model = modelWrapper_.Get(LevelOfDetail::L1);
    if (not model)
    {
        return;
    }

    vec3 wavesScale(1.f, 1.f, 1.f);  // To do get from water parameters

    modelBoundingBox_ = model->getBoundingBox();
    modelBoundingBox_.scale(thisObject_.GetWorldTransform().GetScale() * wavesScale);
    modelBoundingBox_.translate(thisObject_.GetWorldTransform().GetPosition());

    meshBoundingBoxes_.clear();
    for (const auto& mesh : model->GetMeshes())
    {
        auto boundingBox = mesh.getBoundingBox();
        boundingBox.scale(thisObject_.GetWorldTransform().GetScale() * wavesScale);
        boundingBox.translate(thisObject_.GetWorldTransform().GetPosition());
        meshBoundingBoxes_.insert({&mesh, boundingBox});
    }
}
const BoundingBox& WaterRendererComponent::getMeshBoundingBox(const Mesh& mesh) const
{
    auto iter =
        std::find_if(meshBoundingBoxes_.begin(), meshBoundingBoxes_.end(), [&](const auto& pair) { return pair.first == &mesh; });
    if (iter != meshBoundingBoxes_.end())
        return iter->second;

    static BoundingBox defaultBox(BoundingBox::NumericLimits::MaxOppositeValues);
    return defaultBox;
}
const BoundingBox& WaterRendererComponent::getModelBoundingBox() const
{
    return modelBoundingBox_;
}
}  // namespace Components
}  // namespace GameEngine
