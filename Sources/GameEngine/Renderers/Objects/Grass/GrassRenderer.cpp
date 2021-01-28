#include "GrassRenderer.h"

#include <GraphicsApi/ShaderProgramType.h>
#include <Logger/Log.h>

#include <algorithm>

#include "GameEngine/Components/Renderer/Grass/GrassRendererComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Resources/Models/Mesh.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
GrassRenderer::GrassRenderer(RendererContext& context)
    : context_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Grass)
{
}

GrassRenderer::~GrassRenderer()
{
}

void GrassRenderer::init()
{
    shader_.Init();
    InitShaderBuffer();
    DEBUG_LOG("Grass renderer initialized.");
}

void GrassRenderer::render()
{
    if (not shader_.IsReady() or subscribes_.empty())
        return;

    prepareShader();
    context_.graphicsApi_.DisableCulling();

    RenderSubscribes();
    EndRender();
}

void GrassRenderer::subscribe(GameObject& gameObject)
{
    auto grass = gameObject.GetComponent<Components::GrassRendererComponent>();

    if (grass != nullptr)
    {
        subscribes_.push_back({gameObject.GetId(), grass});
    }
}

void GrassRenderer::unSubscribe(GameObject& gameObject)
{
    auto iter = subscribes_.begin();
    while (iter != subscribes_.end())
    {
        if (iter->first == gameObject.GetId())
        {
            iter = subscribes_.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void GrassRenderer::unSubscribeAll()
{
    subscribes_.clear();
}

void GrassRenderer::reloadShaders()
{
    shader_.Reload();
}

void GrassRenderer::InitShaderBuffer()
{
    grassShaderBufferId_ =
        context_.graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION, sizeof(GrassShaderBuffer));

    grassShaderBuffer_.variables.value.x = *EngineConf.renderer.flora.viewDistance;
    grassShaderBuffer_.variables.value.y = 0;

    context_.graphicsApi_.UpdateShaderBuffer(*grassShaderBufferId_, &grassShaderBuffer_);
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

void GrassRenderer::prepareShader()
{
    shader_.Start();
    grassShaderBuffer_.variables.value.y += 0.1f * context_.time_.deltaTime;
    context_.graphicsApi_.UpdateShaderBuffer(*grassShaderBufferId_, &grassShaderBuffer_);
    context_.graphicsApi_.BindShaderBuffer(*grassShaderBufferId_);
}
}  // namespace GameEngine
