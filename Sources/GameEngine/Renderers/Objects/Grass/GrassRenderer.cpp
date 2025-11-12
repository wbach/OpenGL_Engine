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
    viewDistanceChangeSubscription_ = EngineConf.renderer.flora.viewDistance.subscribeForChange(
        [this]()
        {
            if (grassShaderBufferId_)
            {
                context_.gpuLoader_.AddFunctionToCall(
                    [this]()
                    {
                        grassShaderBuffer_.variables.value.x = EngineConf.renderer.flora.viewDistance;
                        grassShaderBuffer_.variables.value.y = 0;
                        context_.graphicsApi_.UpdateShaderBuffer(*grassShaderBufferId_, &grassShaderBuffer_);
                    });
            }
        });
}

GrassRenderer::~GrassRenderer()
{
    EngineConf.renderer.flora.viewDistance.unsubscribe(viewDistanceChangeSubscription_);
}

void GrassRenderer::init()
{
    shader_.Init();
    InitShaderBuffer();
    LOG_DEBUG << "Grass renderer initialized.";
}

void GrassRenderer::render()
{
    if (EngineConf.renderer.flora.isEnabled and shader_.IsReady() and not subscribes_.empty())
    {
        prepareShader();
        context_.graphicsApi_.DisableCulling();

        RenderSubscribes();
        EndRender();
    }
}
void GrassRenderer::subscribe(GameObject& gameObject)
{
    std::lock_guard<std::mutex> lk(subscriberMutex_);
    auto iter            = std::find_if(subscribes_.begin(), subscribes_.end(),
                                        [id = gameObject.GetId()](const auto& pair) { return pair.first == id; });
    auto grassComponents = gameObject.GetComponents<Components::GrassRendererComponent>();

    if (grassComponents.empty())
    {
        return;
    }

    if (iter == subscribes_.end())
    {
        subscribes_.push_back({gameObject.GetId(), std::move(grassComponents)});
    }
    else
    {
        auto& components = iter->second;
        components       = std::move(grassComponents);
    }
}

void GrassRenderer::unSubscribe(GameObject& gameObject)
{
    std::lock_guard<std::mutex> lk(subscriberMutex_);
    auto iter = std::find_if(subscribes_.begin(), subscribes_.end(),
                             [id = gameObject.GetId()](const auto& pair) { return pair.first == id; });

    if (iter != subscribes_.end())
    {
        subscribes_.erase(iter);
    }
}

void GrassRenderer::unSubscribeAll()
{
    std::lock_guard<std::mutex> lk(subscriberMutex_);
    subscribes_.clear();
}

void GrassRenderer::reloadShaders()
{
    shader_.Reload();
}

void GrassRenderer::InitShaderBuffer()
{
    grassShaderBufferId_ = context_.graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION, sizeof(GrassShaderBuffer));

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
    std::lock_guard<std::mutex> lk(subscriberMutex_);

    for (const auto& [_, components] : subscribes_)
    {
        for (const auto& component : components)
        {
            if (auto model = component->GetModel().Get())
            {
                RenderModel(*model);
            }
        }
    }
}

void GrassRenderer::RenderModel(const Model& model)
{
    // int rendererdMeshes = 0;
    for (const auto& mesh : model.GetMeshes())
    {
        if (mesh.GetGraphicsObjectId())
        {
            // auto isVisible = context_.frustrum_.intersection(mesh.getBoundingBox());
            // if (isVisible)
            {
                RenderMesh(mesh);
                //++rendererdMeshes;
            }
        }
    }
    // /* LOG TO FIX*/  LOG_ERROR << ("rendererdMeshes : " + std::to_string(rendererdMeshes));
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
    grassShaderBuffer_.variables.value.y +=  context_.time_.deltaTime;
    context_.graphicsApi_.UpdateShaderBuffer(*grassShaderBufferId_, &grassShaderBuffer_);
    context_.graphicsApi_.BindShaderBuffer(*grassShaderBufferId_);
}
void GrassRenderer::unSubscribe(const Components::IComponent& component)
{
    const auto& gameObject = component.getParentGameObject();
    LOG_DEBUG << "Try unSubscribe component " << gameObject.GetName();

    std::lock_guard<std::mutex> lk(subscriberMutex_);
    auto iter = std::find_if(subscribes_.begin(), subscribes_.end(),
                             [id = gameObject.GetId()](const auto& pair) { return pair.first == id; });

    if (iter != subscribes_.end())
    {
        auto& components = iter->second;

        auto componentIter = std::find(components.begin(), components.end(), &component);
        if (componentIter != components.end())
        {
            LOG_DEBUG << "UnSubscribe component " << (*componentIter)->getTextureFile();
            components.erase(componentIter);
        }

        if (components.empty())
        {
            LOG_DEBUG << "UnSubscribe last component in object " << gameObject.GetName();
            subscribes_.erase(iter);
        }
    }
}
}  // namespace GameEngine
