#include "WaterRendererComponent.h"

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string COMPONENT_STR   = "Water";
const std::string CSTR_COLOR      = "color";
const std::string CSTR_WAVE_SPEED = "waveSpeed";
const std::string CSTR_DUDV_MAP   = "dudv";
const std::string CSTR_NORMAL_MAP = "normalMap";
}  // namespace
WaterRendererComponent::WaterRendererComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(WaterRendererComponent).hash_code(), componentContext, gameObject)
    , moveFactor_(0)
    , waterColor_(Utils::RGBtoFloat(0.f, 44.f, 82.f), 1.f)
    , waveSpeed_(.1f)
    , isSubscribed_(false)
{
}
WaterRendererComponent::~WaterRendererComponent()
{
}
void WaterRendererComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&WaterRendererComponent::OnAwake, this));
}
const vec4& WaterRendererComponent::GetWaterColor() const
{
    return waterColor_;
}
float WaterRendererComponent::moveFactor() const
{
    return moveFactor_;
}
float WaterRendererComponent::increaseAndGetMoveFactor(float deltaTime)
{
    moveFactor_ += waveSpeed_ * deltaTime;
    moveFactor_ = fmodf(moveFactor_, 1.f);
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
    waveSpeed_ = speed;
    return *this;
}
float WaterRendererComponent::GetWaveSpeed() const
{
    return waveSpeed_;
}
void WaterRendererComponent::SetWaterColor(const vec4& color)
{
    waterColor_ = color;
}
void WaterRendererComponent::OnAwake()
{
    if (not isSubscribed_)
    {
        componentContext_.renderersManager_.Subscribe(&thisObject_);
        isSubscribed_ = true;
    }

    if (not perObjectUpdateBuffer_)
    {
        perObjectUpdateBuffer_ = std::make_unique<BufferObject<PerObjectUpdate>>(componentContext_.graphicsApi_,
                                                                                 PER_OBJECT_UPDATE_BIND_LOCATION);
        updatePerObjectUpdateBuffer();
    }

    if (not onTransformChangeSubscribtion_)
    {
        onTransformChangeSubscribtion_ = thisObject_.SubscribeOnWorldTransfomChange(
            [this](const common::Transform&) { updatePerObjectUpdateBuffer(); });
    }
}
void WaterRendererComponent::UnSubscribe()
{
    if (isSubscribed_)
    {
        componentContext_.renderersManager_.UnSubscribe(&thisObject_);
        isSubscribed_ = false;
    }
}
void WaterRendererComponent::updatePerObjectUpdateBuffer()
{
    if (not perObjectUpdateBuffer_)
        return;

    DEBUG_LOG("update buffer");

    const auto& transform = thisObject_.GetWorldTransform();

    auto convertedQuadScale = transform.GetScale();
    convertedQuadScale.y = convertedQuadScale.z;
    convertedQuadScale.z = 1.f;
    auto transformMatrix =
        Utils::CreateTransformationMatrix(transform.GetPosition(), DegreesVec3(-90, 0, 0), convertedQuadScale);

    perObjectUpdateBuffer_->GetData().TransformationMatrix =
        componentContext_.graphicsApi_.PrepareMatrixToLoad(transformMatrix);

    if (not perObjectUpdateBuffer_->GetGraphicsObjectId())
    {
        componentContext_.gpuResourceLoader_.AddObjectToGpuLoadingPass(*perObjectUpdateBuffer_);
    }
    else
    {
        componentContext_.gpuResourceLoader_.AddObjectToUpdateGpuPass(*perObjectUpdateBuffer_);
    }
}
void WaterRendererComponent::CleanUp()
{
    UnSubscribe();

    if (onTransformChangeSubscribtion_)
    {
        thisObject_.UnsubscribeOnWorldTransfromChange(*onTransformChangeSubscribtion_);
    }
    if (perObjectUpdateBuffer_)
    {
        componentContext_.gpuResourceLoader_.AddObjectToRelease(std::move(perObjectUpdateBuffer_));
    }

    DeleteTextures();
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
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
        auto component = std::make_unique<WaterRendererComponent>(componentContext, gameObject);

        float waveSpeed{1.f};
        vec4 color(0, 0, 0.4f, 0.5f);
        std::string dudvMap, normalMap;
        ::Read(node.getChild(CSTR_WAVE_SPEED), waveSpeed);
        ::Read(node.getChild(CSTR_COLOR), color);
        ::Read(node.getChild(CSTR_DUDV_MAP), dudvMap);
        ::Read(node.getChild(CSTR_NORMAL_MAP), normalMap);

        component->SetWaveSpeed(waveSpeed);
        component->SetWaterColor(color);
        if (not dudvMap.empty() and not normalMap.empty())
        {
            component->LoadTextures(dudvMap, normalMap);
        }
        return component;
    };

    regsiterComponentReadFunction(COMPONENT_STR, readFunc);
}
void WaterRendererComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});

    ::write(node.addChild(CSTR_COLOR), GetWaterColor());
    ::write(node.addChild(CSTR_WAVE_SPEED), GetWaveSpeed());

    if (GetDudvTexture() and GetDudvTexture()->GetFile())
        ::write(node.addChild(CSTR_DUDV_MAP), GetDudvTexture()->GetFile()->GetDataRelativeDir());

    if (GetNormalTexture() and GetNormalTexture()->GetFile())
        ::write(node.addChild(CSTR_NORMAL_MAP), GetNormalTexture()->GetFile()->GetDataRelativeDir());
}
}  // namespace Components
}  // namespace GameEngine
