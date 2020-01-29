#include "ShadowMapRenderer.hpp"
#include "GLM/GLMUtils.h"
#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Framebuffer/DeferedFrameBuffer/DeferedFrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerFrameBuffer.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "GraphicsApi/ShadersTypes.h"
#include "Logger/Log.h"
#include "Shaders/ShadowShaderUniforms.h"
#include "ShadowFrameBuffer.h"
#include "math.hpp"

namespace GameEngine
{
ShadowMapRenderer::ShadowMapRenderer(RendererContext& context)
    : context_(context)
    , shadowBox_(context.projection_)
    , projectionViewMatrix_(1.f)
    , viewOffset_(Utils::CreateOffset())
    , isInit_(false)
{
    shader_ = context.shaderFactory_.create(GraphicsApi::Shaders::Shadows);
    __RegisterRenderFunction__(RendererFunctionType::PRERENDER, ShadowMapRenderer::Render);
}

ShadowMapRenderer::~ShadowMapRenderer()
{
    if (perFrameBuffer_)
    {
        context_.graphicsApi_.DeleteShaderBuffer(*perFrameBuffer_);
    }
}

void ShadowMapRenderer::Init()
{
    isInit_ = shader_->Init();

    perFrameBuffer_  = context_.graphicsApi_.CreateShaderBuffer(PER_FRAME_BIND_LOCATION, sizeof(PerFrameBuffer));

    if (not isInit_ or not perFrameBuffer_)
    {
        ERROR_LOG("ShadowMapRenderer inir error");
    }
}

void ShadowMapRenderer::Render(const Scene& scene, const Time&)
{
    if (not isInit_ or not perFrameBuffer_)
        return;

    uint32 lastBindedPerFrameBuffer  = context_.graphicsApi_.BindShaderBuffer(*perFrameBuffer_);

    PrepareRender(scene);
    shader_->Start();
    RenderSubscribes();
    context_.shadowsFrameBuffer_.UnbindFrameBuffer();

    context_.graphicsApi_.BindShaderBuffer(lastBindedPerFrameBuffer);
}

void ShadowMapRenderer::Subscribe(GameObject* gameObject)
{
    auto rendererComponent = gameObject->GetComponent<Components::RendererComponent>();

    if (rendererComponent == nullptr)
        return;

    subscribes_.push_back({gameObject, rendererComponent});
}

void ShadowMapRenderer::UnSubscribe(GameObject* gameObject)
{
    for (auto iter = subscribes_.begin(); iter != subscribes_.end();)
    {
        if ((*iter).gameObject->GetId() == gameObject->GetId())
        {
            iter = subscribes_.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void ShadowMapRenderer::UnSubscribeAll()
{
    subscribes_.clear();
}

void ShadowMapRenderer::ReloadShaders()
{
    shader_->Stop();
    shader_->Reload();
    shader_->Init();
}

void ShadowMapRenderer::PrepareRender(const Scene& scene)
{
    context_.shadowsFrameBuffer_.BindFBO();
    context_.graphicsApi_.EnableDepthTest();
    context_.graphicsApi_.ClearBuffer(GraphicsApi::BufferType::DEPTH);
    shadowBox_.Update(scene.GetCamera());

    auto light_direction = scene.GetDirectionalLight().GetDirection();
    shadowBox_.CalculateMatrixes(light_direction);

    context_.toShadowMapZeroMatrix_ = viewOffset_ * shadowBox_.GetProjectionViewMatrix();

    PerFrameBuffer perFrame;
    perFrame.ProjectionViewMatrix = shadowBox_.GetProjectionViewMatrix();

    context_.graphicsApi_.UpdateShaderBuffer(*perFrameBuffer_, &perFrame);
}

void ShadowMapRenderer::RenderSubscribes() const
{
    for (auto& sub : subscribes_)
        RenderSubscriber(sub);
}

void ShadowMapRenderer::RenderSubscriber(const ShadowMapSubscriber& sub) const
{
    auto model = sub.renderComponent->GetModelWrapper().Get(LevelOfDetail::L1);

    if (model == nullptr)
        return;

    const auto& meshes                    = model->GetMeshes();

    int meshId = 0;
    for (const auto& mesh : meshes)
    {
        if (not mesh.IsInit())
            continue;

        const auto& buffers = mesh.GetBuffers();

        context_.graphicsApi_.BindShaderBuffer(*buffers.perMeshObjectBuffer_);

        if (mesh.UseArmature())
        {
            context_.graphicsApi_.BindShaderBuffer(*buffers.perPoseUpdateBuffer_);
        }

        const auto& perMeshUpdateBuffer = sub.renderComponent->GetPerObjectUpdateBuffer(meshId);
        if (perMeshUpdateBuffer)
        {
            context_.graphicsApi_.BindShaderBuffer(*perMeshUpdateBuffer);
        }

        const auto& perMeshConstantBuffer = sub.renderComponent->GetPerObjectConstantsBuffer(meshId);
        if (perMeshConstantBuffer)
        {
            context_.graphicsApi_.BindShaderBuffer(*perMeshConstantBuffer);
        }
        ++meshId;
        RenderMesh(mesh);
    }
}

void ShadowMapRenderer::RenderMesh(const Mesh& mesh) const
{
    if (!mesh.IsInit())
        return;

    context_.graphicsApi_.ActiveTexture(0, mesh.GetMaterial().diffuseTexture->GetId());
    context_.graphicsApi_.RenderMesh(mesh.GetObjectId());
}

}  // namespace GameEngine
