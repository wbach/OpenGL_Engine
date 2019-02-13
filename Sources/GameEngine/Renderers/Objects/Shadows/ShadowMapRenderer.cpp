#include "ShadowMapRenderer.hpp"
#include "GLM/GLMUtils.h"
#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Framebuffer/DeferedFrameBuffer/DeferedFrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerFrameBuffer.h"
#include "GameEngine/Resources/ShaderBuffers/PerResizeBuffer.h"
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
    , shadowBox2_(context.projection_)
    , projectionViewMatrix_(1.f)
    , viewOffset_(Utils::CreateOffset())
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

    if (perResizeBuffer_)
    {
        context_.graphicsApi_.DeleteShaderBuffer(*perResizeBuffer_);
    }
}

void ShadowMapRenderer::Init()
{
    shader_->Init();
    perResizeBuffer_ = context_.graphicsApi_.CreateShaderBuffer(PER_RESIZE_BIND_LOCATION, sizeof(PerResizeBuffer));
    perFrameBuffer_  = context_.graphicsApi_.CreateShaderBuffer(PER_FRAME_BIND_LOCATION, sizeof(PerFrameBuffer));
}

void ShadowMapRenderer::Render(Scene* scene)
{
    if (not perResizeBuffer_ or not perFrameBuffer_)
        return;

    uint32 lastBindedPerResizeBuffer = context_.graphicsApi_.BindShaderBuffer(*perResizeBuffer_);
    uint32 lastBindedPerFrameBuffer  = context_.graphicsApi_.BindShaderBuffer(*perFrameBuffer_);

    PrepareRender(scene);
    PrepareShader(scene->GetCamera());
    RenderSubscribes();
    context_.shadowsFrameBuffer_.UnbindFrameBuffer();

    context_.graphicsApi_.BindShaderBuffer(lastBindedPerResizeBuffer);
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

void ShadowMapRenderer::PrepareRender(Scene* scene)
{
    context_.shadowsFrameBuffer_.BindFBO();
    context_.graphicsApi_.EnableDepthTest();
    context_.graphicsApi_.ClearBuffer(GraphicsApi::BufferType::DEPTH);
    shadowBox_.Update(scene->GetCamera());

    auto cameraPos = scene->GetCamera()->GetPosition();
    scene->GetCamera()->SetPosition(vec3(0, 0, 0));
    shadowBox2_.Update(scene->GetCamera());
    scene->GetCamera()->SetPosition(cameraPos);

    auto light_direction = scene->GetDirectionalLight().GetDirection();
    shadowBox_.CalculateMatrixes(light_direction);
    shadowBox2_.CalculateMatrixes(light_direction);

    context_.toShadowMapZeroMatrix_ = viewOffset_ * shadowBox_.GetProjectionViewMatrix();

    PerResizeBuffer perResize;
    perResize.ProjectionMatrix = shadowBox_.GetProjectionMatrix();

    context_.graphicsApi_.UpdateShaderBuffer(*perResizeBuffer_, &perResize);

    PerFrameBuffer perFrame;
    perFrame.ViewMatrix = shadowBox_.GetViewMatrix();

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
    const auto& perObjectUpdateBuffers    = sub.renderComponent->GetPerObjectUpdateBuffers();
    const auto& perObjectConstantsBuffers = sub.renderComponent->GetPerObjectConstantsBuffers();

    int meshId = 0;
    for (const auto& mesh : meshes)
    {
        const auto& buffers = mesh.GetBuffers();

        context_.graphicsApi_.BindShaderBuffer(*buffers.perMeshObjectBuffer_);

        if (mesh.UseArmature())
        {
            context_.graphicsApi_.BindShaderBuffer(*buffers.perPoseUpdateBuffer_);
        }

        const auto& perMeshUpdateBuffer = perObjectUpdateBuffers[meshId].GetId();
        if (perMeshUpdateBuffer)
        {
            context_.graphicsApi_.BindShaderBuffer(*perMeshUpdateBuffer);
        }

        const auto& perMeshConstantBuffer = perObjectConstantsBuffers[meshId].GetId();
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

void ShadowMapRenderer::PrepareShader(ICamera*) const
{
    shader_->Start();
}
}  // namespace GameEngine
