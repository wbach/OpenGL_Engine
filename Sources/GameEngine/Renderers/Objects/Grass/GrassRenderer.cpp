#include "GrassRenderer.h"

#include <algorithm>

#include "GameEngine/Components/Renderer/Grass/GrassComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Resources/Models/Mesh.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GraphicsApi/ShaderProgramType.h"
#include "Logger/Log.h"

namespace GameEngine
{
GrassRenderer::GrassRenderer(RendererContext& context)
    : context_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Grass)
{
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, GrassRenderer::Render);
}

GrassRenderer::~GrassRenderer()
{
}

void GrassRenderer::Init()
{
    shader_.Init();
    InitShaderBuffer();
    DEBUG_LOG("Grass renderer initialized.");
}

void GrassRenderer::Render(const Scene& scene, const Time&)
{
    if (not shader_.IsReady() or subscribes_.empty())
        return;

    PrepareRender(scene);
    RenderSubscribes();
    EndRender();
}

void GrassRenderer::Subscribe(GameObject* gameObject)
{
    auto grass = gameObject->GetComponent<Components::GrassRendererComponent>();

    if (grass != nullptr)
    {
        subscribes_.push_back({gameObject->GetId(), grass});
    }
}

void GrassRenderer::UnSubscribe(GameObject* gameObject)
{
    auto iter = subscribes_.begin();
    while (iter != subscribes_.end())
    {
        if (iter->first == gameObject->GetId())
        {
            iter = subscribes_.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void GrassRenderer::UnSubscribeAll()
{
    subscribes_.clear();
}

void GrassRenderer::ReloadShaders()
{
    shader_.Reload();
}

void GrassRenderer::InitShaderBuffer()
{
    grassShaderBufferId_ =
        context_.graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION, sizeof(GrassShaderBuffer));

    grassShaderBuffer_.variables.value.x = EngineConf.renderer.flora.viewDistance;
    grassShaderBuffer_.variables.value.y = 0;

    context_.graphicsApi_.UpdateShaderBuffer(*grassShaderBufferId_, &grassShaderBuffer_);
}

void GrassRenderer::PrepareRender(const Scene& scene)
{
    PrepareShader(scene);
    context_.graphicsApi_.DisableCulling();
}

void GrassRenderer::EndRender() const
{
    context_.graphicsApi_.EnableCulling();
}

void GrassRenderer::RenderSubscribes()
{
    for (const auto& s : subscribes_)
    {
        auto model = s.second->GetModel().Get();

        if (not model)
            continue;

        RenderModel(*model);
    }
}

void GrassRenderer::RenderModel(const Model& model)
{
    for (const auto& mesh : model.GetMeshes())
    {
        if (mesh.GetGraphicsObjectId())
            RenderMesh(mesh);
    }
}

void GrassRenderer::RenderMesh(const Mesh& mesh)
{
    auto diffTexture = mesh.GetMaterial().diffuseTexture;

    if (diffTexture and diffTexture->GetGraphicsObjectId())
        context_.graphicsApi_.ActiveTexture(0, *diffTexture->GetGraphicsObjectId());

    context_.graphicsApi_.RenderPoints(*mesh.GetGraphicsObjectId());
}

void GrassRenderer::PrepareShader(const Scene& scene)
{
    shader_.Start();
    grassShaderBuffer_.variables.value.y = scene.GetGlobalTime();
    context_.graphicsApi_.UpdateShaderBuffer(*grassShaderBufferId_, &grassShaderBuffer_);
    context_.graphicsApi_.BindShaderBuffer(*grassShaderBufferId_);
}
}  // namespace GameEngine
