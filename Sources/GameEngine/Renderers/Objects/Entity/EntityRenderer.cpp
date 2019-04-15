#include "EntityRenderer.h"
#include "EntityRendererDef.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Framebuffer/DeferedFrameBuffer/DeferedFrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "GraphicsApi/ShadersTypes.h"
#include "Logger/Log.h"
#include "Shaders/EntityShaderUniforms.h"

namespace GameEngine
{
EntityRenderer::EntityRenderer(RendererContext& context)
    : context_(context)
{
    shader_ = context.shaderFactory_.create(GraphicsApi::Shaders::Entity);
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, EntityRenderer::Render);
}

EntityRenderer::~EntityRenderer()
{
    UnSubscribeAll();
}

void EntityRenderer::Init()
{
    DEBUG_LOG("Start initialize entity renderer...");
    InitShader();
    DEBUG_LOG("EntityRenderer initialized.");
}

void EntityRenderer::InitShader()
{
    shader_->Init();
}

void EntityRenderer::Render(const Scene&, const Time&)
{
    if (subscribes_.empty())
        return;
    shader_->Start();
    RenderEntities();
}

void EntityRenderer::Subscribe(GameObject* gameObject)
{
    auto rendererComponent = gameObject->GetComponent<Components::RendererComponent>();

    if (rendererComponent == nullptr)
        return;

    subscribes_.push_back({gameObject, rendererComponent});
}

void EntityRenderer::UnSubscribe(GameObject* gameObject)
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

void EntityRenderer::UnSubscribeAll()
{
    subscribes_.clear();
}

void EntityRenderer::ReloadShaders()
{
    shader_->Stop();
    shader_->Reload();
    InitShader();
}

void EntityRenderer::RenderEntities()
{
    for (const auto& sub : subscribes_)
    {
        const auto& rcomp = sub.renderComponent;
        auto model        = rcomp->GetModelWrapper().Get(LevelOfDetail::L1);

        if (model == nullptr)
            continue;

        RenderModel(sub, *model);
    }
}

void EntityRenderer::RenderModel(const EntitySubscriber& subsriber, const Model& model) const
{
    const auto& meshes                    = model.GetMeshes();

    int meshId = 0;
    for (const auto& mesh : meshes)
    {
        const auto& buffers = mesh.GetBuffers();

        context_.graphicsApi_.BindShaderBuffer(*buffers.perMeshObjectBuffer_);

        if (mesh.UseArmature())
        {
            context_.graphicsApi_.BindShaderBuffer(*buffers.perPoseUpdateBuffer_);
        }

        const auto& perMeshUpdateBuffer = subsriber.renderComponent->GetPerObjectUpdateBuffer(meshId);
        if (perMeshUpdateBuffer)
        {
            context_.graphicsApi_.BindShaderBuffer(*perMeshUpdateBuffer);
        }
        
        const auto& perMeshConstantBuffer = subsriber.renderComponent->GetPerObjectConstantsBuffer(meshId);
        if (perMeshConstantBuffer)
        {
            context_.graphicsApi_.BindShaderBuffer(*perMeshConstantBuffer);
        }
        ++meshId;
        RenderMesh(mesh);
    }
}

void EntityRenderer::RenderMesh(const Mesh& mesh) const
{
    BindMaterial(mesh.GetMaterial());
    context_.graphicsApi_.RenderMesh(mesh.GetObjectId());
    UnBindMaterial(mesh.GetMaterial());
}

void EntityRenderer::BindMaterial(const Material& material) const
{
    if (material.isTransparency)
        context_.graphicsApi_.DisableCulling();

    if (material.diffuseTexture != nullptr && material.diffuseTexture->IsInitialized() &&
        EngineConf.renderer.textures.useDiffuse)
    {
        context_.graphicsApi_.ActiveTexture(0, material.diffuseTexture->GetId());
    }

    if (material.ambientTexture != nullptr && material.ambientTexture->IsInitialized() &&
        EngineConf.renderer.textures.useAmbient)
    {
        context_.graphicsApi_.ActiveTexture(1, material.ambientTexture->GetId());
    }

    if (material.normalTexture != nullptr && material.normalTexture->IsInitialized() &&
        EngineConf.renderer.textures.useNormal)
    {
        context_.graphicsApi_.ActiveTexture(2, material.normalTexture->GetId());
    }

    if (material.specularTexture != nullptr && material.specularTexture->IsInitialized() &&
        EngineConf.renderer.textures.useSpecular)
    {
        context_.graphicsApi_.ActiveTexture(3, material.specularTexture->GetId());
    }
}

void EntityRenderer::UnBindMaterial(const Material& material) const
{
    if (material.isTransparency)
        context_.graphicsApi_.EnableCulling();
}
}  // namespace GameEngine
