#include "PreviewRenderer.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "GameEngine/Components/Renderer/Entity/PreviewComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Projection/IProjection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerFrameBuffer.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
PreviewRenderer::PreviewRenderer(RendererContext& context)
    : context_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::PreviewEntity)
    , frameBuffer_(nullptr)
    , isInit_(false)
{
}
PreviewRenderer::~PreviewRenderer()
{
}
void PreviewRenderer::init()
{
    shader_.Init();

    const vec2ui size{1024, 1024};

    GraphicsApi::FrameBuffer::Attachment colorAttachment(size, GraphicsApi::FrameBuffer::Type::Color0,
                                                         GraphicsApi::FrameBuffer::Format::Rgba8);

    colorAttachment.filter       = GraphicsApi::FrameBuffer::Filter::Linear;
    colorAttachment.defaultValue = vec4(0.8, 0.8, 0.8, 1.f);

    GraphicsApi::FrameBuffer::Attachment depthAttachment(size, GraphicsApi::FrameBuffer::Type::Depth,
                                                         GraphicsApi::FrameBuffer::Format::Depth);

    depthAttachment.wrapMode = GraphicsApi::FrameBuffer::WrapMode::ClampToEdge;
    depthAttachment.filter   = GraphicsApi::FrameBuffer::Filter::Linear;

    frameBuffer_ = &context_.graphicsApi_.CreateFrameBuffer({depthAttachment, colorAttachment});
    isInit_      = frameBuffer_->Init();

    if (not perFrameBufferId_)
    {
        perFrameBufferId_ = context_.graphicsApi_.CreateShaderBuffer(PER_FRAME_BIND_LOCATION, sizeof(PerFrameBuffer));
    }
}
void PreviewRenderer::subscribe(GameObject& gameObject)
{
    auto previewComponent = gameObject.GetComponent<Components::PreviewComponent>();

    if (previewComponent)
    {
        subcribers_.insert({gameObject.GetId(), previewComponent});
    }
}
void PreviewRenderer::unSubscribe(uint32 id)
{
    subcribers_.erase(id);
}
void PreviewRenderer::unSubscribe(GameObject& gameObject)
{
    unSubscribe(gameObject.GetId());
}
void PreviewRenderer::unSubscribeAll()
{
    subcribers_.clear();
}
void PreviewRenderer::reloadShaders()
{
    shader_.Reload();
}
void PreviewRenderer::prepare()
{
    if (not isInit_ or not shader_.IsReady())
        return;
    std::vector<uint32> doneObjects;

    context_.graphicsApi_.EnableDepthTest();

    for (auto& subcriber : subcribers_)
    {
        auto model = subcriber.second->GetModelWrapper().Get();

        if (model)
        {
            if (model->GetMeshes().empty() or not std::all_of(model->GetMeshes().begin(), model->GetMeshes().end(),
                                                              [](const auto& mesh) { return mesh.GetGraphicsObjectId(); }))
            {
                continue;
            }

            frameBuffer_->Clear();
            frameBuffer_->Bind(GraphicsApi::FrameBuffer::BindType::ReadWrite);
            shader_.Start();
            LOG_DEBUG << "Render mesh : " << subcriber.second->getOutputFile();

            for (const auto& mesh : model->GetMeshes())
            {
                if (not mesh.GetGraphicsObjectId())
                    continue;

                vec3 cameraPosition(1.5f);
                auto rotation             = Utils::lookAt(vec3(0.5f, 0, 0.f), cameraPosition);
                auto viewMatrix           = Utils::createViewMatrix(rotation, cameraPosition);
                auto projectionViewMatrix = glm::perspective(glm::radians(50.f), 1.f, 0.1f, 1000.f) * viewMatrix;

                PerFrameBuffer perFrameBuffer;
                perFrameBuffer.ProjectionViewMatrix =
                    context_.graphicsApi_.PrepareMatrixToLoad(projectionViewMatrix * mesh.GetMeshTransform());
                perFrameBuffer.cameraPosition = cameraPosition;
                perFrameBuffer.clipPlane      = vec4(0.f, -1.f, 0.f, 10000.f);
                perFrameBuffer.projection     = context_.camera_->GetProjection().GetBufferParams();
                context_.graphicsApi_.UpdateShaderBuffer(*perFrameBufferId_, &perFrameBuffer);
                auto lastBindedShaderBuffer = context_.graphicsApi_.BindShaderBuffer(*perFrameBufferId_);

                bindMaterial(mesh.GetMaterial());
                context_.graphicsApi_.RenderMesh(*mesh.GetGraphicsObjectId());
                unBindMaterial(mesh.GetMaterial());

                context_.graphicsApi_.BindShaderBuffer(lastBindedShaderBuffer);
            }

            frameBuffer_->TakeSnapshot(subcriber.second->getOutputFile().GetAbsolutePath().parent_path().string());

            auto outputFileName =
                subcriber.second->getOutputFile().GetAbsolutePath() /
                (std::to_string(*frameBuffer_->GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Color0)) + ".png");

            LOG_DEBUG << outputFileName;
            LOG_DEBUG << subcriber.second->getOutputFile();

            Utils::RenameFile(outputFileName.string(), subcriber.second->getOutputFile().GetAbsolutePath().string());

            doneObjects.push_back(subcriber.first);
            shader_.Stop();
            frameBuffer_->UnBind();
        }
    }

    for (auto id : doneObjects)
    {
        unSubscribe(id);
        context_.scene_->RemoveGameObject(id);
    }
}
void PreviewRenderer::bindMaterial(const Material& material) const
{
    if (material.isTransparency)
        context_.graphicsApi_.DisableCulling();

    const auto& config = EngineConf.renderer.textures;
    bindMaterialTexture(0, material.diffuseTexture, config.useDiffuse);
    bindMaterialTexture(1, material.ambientTexture, config.useAmbient);
    bindMaterialTexture(2, material.normalTexture, config.useNormal);
    bindMaterialTexture(3, material.specularTexture, config.useSpecular);
}
void PreviewRenderer::unBindMaterial(const Material& material) const
{
    if (material.isTransparency)
        context_.graphicsApi_.EnableCulling();
}

void PreviewRenderer::bindMaterialTexture(uint32 location, Texture* texture, bool enabled) const
{
    if (enabled and texture and texture->GetGraphicsObjectId())
    {
        context_.graphicsApi_.ActiveTexture(location, *texture->GetGraphicsObjectId());
    }
}
}  // namespace GameEngine
